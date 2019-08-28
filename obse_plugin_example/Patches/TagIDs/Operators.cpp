#include "./Main.h"
#include "./Operators.h"
#include "obse_common/SafeWrite.h"
#include "shared.h"

#include "Fun/x86Reader.h"
#include "Miscellaneous/math.h"
#include "ReverseEngineered/NetImmerse/NiTypes.h"
#include "ReverseEngineered/UI/Tile.h"
#include "Patches/CleanUpAfterMenuQue.h"
#include "Services/INISettings.h"
#include "Services/UIPrefs.h"

#include <cmath>

namespace CobbPatches {
   namespace TagIDs {
      namespace Operators {
         namespace StringOperandSupport {
            //
            // Add support for string operands in operators, i.e. the ability of an operator in 
            // UI XML to hold a string value in-memory at run-time.
            //
            namespace ExpressionDestructor {
               //
               // Free strings from memory when their owning operators die. MenuQue doesn't do 
               // this, probably for maximum safety in case something clobbers the byte that they 
               // and we co-opt as an "is string" bool, but I don't want to leak every string 
               // that ever gets stuffed into an operand.
               //
               void _stdcall Inner(RE::Tile::Value::Expression* op) {
                  if (op->isString && op->operand.string) {
                     FormHeap_Free((void*)op->operand.string);
                     op->operand.string = nullptr;
                     op->isString = false;
                  }
               }
               __declspec(naked) void Outer() {
                  _asm {
                     push ecx;
                     call Inner; // stdcall
                     pop  esi; // reproduce patched-over instruction
                     retn;     // reproduce patched-over instruction
                  };
               }
               void Apply() {
                  WriteRelJump(0x0058892B, (UInt32)&Outer);
               }
            }
            namespace ExpressionRemove {
               //
               // Free strings from memory if their owning operators are removed. This typically 
               // happens if a trait's value is overwritten with a constant at run-time. As above, 
               // MenuQue doesn't do this.
               //
               void Inner(RE::Tile::Value::Expression* op) {
                  if (op->isString && op->operand.string) {
                     FormHeap_Free((void*)op->operand.string);
                     op->operand.string = nullptr;
                     op->isString = false;
                  }
                  FormHeap_Free((void*)op);
               }
               void Apply() {
                  WriteRelCall(0x0058898A, (UInt32)&Inner);
               }

            }
            namespace UpdateTemplatedChildren {
               //
               // MenuQue's approach to handling string operators during parsing is to patch 
               // UpdateTemplatedChildren to handle the ConstOperator token differently: they 
               // check if the token's float value is zero and if its string value has a non-
               // zero length, and if so, they treat the ConstOperator as an operator with a 
               // string operand; otherwise, they use the vanilla behavior.
               //
               // By contrast, we've created a new token for const operators with string 
               // operands, and we assign this token in our replacement for TileTemplate's 
               // AddTemplateItem method. We patch UpdateTemplatedChildren to handle the new 
               // token as a totally separate branch from ConstOperator.
               //
               void _stdcall Inner(RE::Tile* tile, UInt32 traitID, RE::Tile::TileTemplateItem* item) {
                  if (!traitID) {
                     _MESSAGE("[UpdateTemplatedChildren] Const string operator defined outside of any trait.");
                     return;
                  }
                  auto trait = CALL_MEMBER_FN(tile, GetOrCreateTrait)(traitID);
                  trait->AppendStringOperator(item->result, item->string.m_data);
               }
               __declspec(naked) void Outer() {
                  _asm {
                     cmp  eax, 0x37; // kCode_ConstOperator
                     je   lConstFloatOperator;
                     cmp  eax, 0x38; // kCode_XXNConstStringOperator
                     jne  lNextBranch;
                     push esi;
                     push ebx;
                     push ebp;
                     call Inner; // stdcall
                     mov  ecx, 0x0058D1A0; // continue
                     jmp  ecx;
                  lConstFloatOperator:
                     mov  ecx, 0x0058D0F4;
                     jmp  ecx;
                  lNextBranch:
                     mov  ecx, 0x0058D126;
                     jmp  ecx;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0058D0EF, (UInt32)&Outer);
               }
            }
            //
            void Apply() {
               ExpressionDestructor::Apply();
               ExpressionRemove::Apply();
               UpdateTemplatedChildren::Apply();
            }
         };
         namespace DoActionEnumeration {
            //
            // NOTE: We haven't managed to fully implement string operators, so 
            // ours don't work. They act as though the current working value and 
            // the argument are both the string " ". I've retained their code for 
            // now, to leave room for improvement in the future. That said, it's 
            // not likely that I'll get them working: any implementation would 
            // have to be MenuQue-compatible, and there's only so much room to 
            // work on that. I'd end up having to completely parrot MenuQue, and 
            // that's not fun.
            //
            // To be specific: as of NorthernUI v2.0.0 we have string operands, 
            // but this isn't the same thing as string operators.
            //
            //  - If container operators didn't exist, then it would be sufficient 
            //    to just write to the Tile::Value whose value is currently being 
            //    computed, and so string operators e.g. append would be possible.
            //
            //  - However, container operators *do* exist, and the "current work-
            //    ing value" differs for each level of nesting. The vanilla game 
            //    handles this for floats by maintaining a list of floats per 
            //    nesting depth; we'd need to patch the game to do the same thing 
            //    for strings. MenuQue does that and I don't want to interfere, 
            //    so we lose out on string operators. Fortunately we don't need 
            //    them for NorthernUI proper; when necessary we can just smash 
            //    multiple text tiles together.
            //
            namespace HandleMenuQue {
               //
               // If we don't handle MenuQue, the game will crash.
               //
               /*//
               struct Patch {
                  const char* version;
                  UInt32 hookTarget; // where does the hook jump to?
                  UInt32 subToCall;
               };
               const Patch knownVersions[] = {
                  { "16a", 0x000125A0, 0x00012180 },
                  { "16b", 0x00012990, 0x00012570 },
               };
               //*/

               typedef void(__stdcall *f_MQOperatorHandler)(RE::Tile::Value* kThis, RE::Tile::Value* esp44, RE::Tile::Value::Expression* currentExpr, UInt32 operatorID);
               static f_MQOperatorHandler mqOperatorHandler = nullptr;
               //
               UInt32 _getJumpTarget(UInt32 jumpFrom) { // arg should be address of 0xE9 byte
                  if (*(UInt8*)jumpFrom != 0xE9)
                     return 0;
                  return *(UInt32*)(jumpFrom + 1) + (jumpFrom + 1) + sizeof(void*);
               }
               void Apply() {
                  _MESSAGE(" - Checking for MenuQue hooks here...");
                  if (!g_menuQue.detected)
                     return;
                  UInt32 jumpAbs = _getJumpTarget(0x0058C33E);
                  if (!jumpAbs) {
                     _MESSAGE(" - We detected MenuQue, but theres's no hook at Tile::Value::DoActionEnumeration+0x45E. Aborting operator safety patch; if you experience crashes, contact NorthernUI's author.");
                     return;
                  }
                  _MESSAGE(" - Detected hook to Tile::Value::DoActionEnumeration+0x45E. Checking to see if it's a known or identifiable MenuQue hook.");
                  //
                  if (g_menuQue.fingerprintedVersion == MenuQueState::kFingerprintedVersion_16b) {
                     mqOperatorHandler = (f_MQOperatorHandler)(g_menuQue.addrBase + 0x00012570);
                     _MESSAGE("    - Assumed MenuQue's operator handler is at MQ:0x%08X per data for known MenuQue version %s.", 0x00012570, "v16b");
                     return;
                  }
                  UInt32 jumpRel = jumpAbs - g_menuQue.addrBase;
                  if (jumpAbs >= g_menuQue.addrBase && (!g_menuQue.addrSize || jumpAbs < (g_menuQue.addrBase + g_menuQue.addrSize))) {
                     _MESSAGE("    - Not a known MenuQue hook. Jump target read as MQ_%08X (%08X).", jumpRel, jumpAbs);
                  } else {
                     _MESSAGE("    - Not a MenuQue hook. Jump target read as %08X.", jumpAbs);
                  }
               };
            };
            //
            bool Inner(const UInt16 operatorID, RE::Tile::Value* const kThis, RE::Tile::Value* const esp44, const float argument, RE::Tile::Value::Expression* const current) {
               //
               // This subroutine should generally only handle our custom operators, since we just 
               // jump back to the vanilla code for vanilla operators.
               //
               if (HandleMenuQue::mqOperatorHandler) {
                  //
                  // This call always has to be made, even if we're not executing a MenuQue operator. 
                  // I suspect it plays a role in managing MenuQue's "string stack" or somesuch.
                  //
                  (HandleMenuQue::mqOperatorHandler)(kThis, esp44, current, operatorID);
                  if (operatorID == RE::kTagID_copy || (operatorID >= 0x7EC && operatorID <= 0x7F0))
                     return true;
               }
               if (operatorID == 0xF) {
                  //
                  // Seen in LootMenu; causes the game to log an error about unrecognized action enumeration 15.
                  //
                  auto tile = kThis->owner;
                  _MESSAGE("Stray 0xF operator in trait %s::%s. This can happen if a SRC operator is not self-closing, e.g. <copy src=\"foo\" trait=\"bar\"></copy>. This should be considered a syntax error.\nExpect the game to log an error about an unrecognized action enumeration.", tile->name.m_data, RE::TagIDToName(kThis->id));
               }
               switch (operatorID) {
                  //
                  // Cases for custom operators should always return true.
                  //
                  // If we wanted to conditionally override a vanilla operator's behavior for some reason, 
                  // we'd return true if we use the custom behavior, or return false to fall through to the 
                  // vanilla behavior.
                  //
                  // To return a value, write it to kThis->num.
                  //
                  // The current wokring value is kThis->num, prior to your writing to it. The float operand 
                  // for your operator (i.e. the argument) is (argument). To access a string operand, go 
                  // through (current).
                  //
                  case _traitOpAtan2:
                     kThis->num = cobb::radians_to_degrees(std::atan2(kThis->num, argument)); // <copy> y </copy> <xxnOpAtan2> x </xxnOpAtan2>
                     return true;
                  case _traitOpCosine:
                     kThis->num = std::cos(cobb::degrees_to_radians(argument));
                     return true;
                  case _traitOpSine:
                     kThis->num = std::sin(cobb::degrees_to_radians(argument));
                     return true;
                  case _traitOpTangent:
                     kThis->num = std::tan(cobb::degrees_to_radians(argument));
                     return true;
                  case _traitOpStringEq:
                     {
                        bool comparison = kThis->str.IsEqual(&esp44->str, false); // This is actually wrong. esp44 is an XML switch-case, e.g. <_example_123 />
                        kThis->num = comparison ? RE::kEntityID_true : RE::kEntityID_false;
                     }
                     return true;
                  case _traitOpStringAppend:
                     kThis->bIsNum = false;
                     {
                        std::string a;
                        std::string b;
                        kThis->str.ToStdString(&a);
                        esp44->str.ToStdString(&b); // This is actually wrong. esp44 is an XML switch-case, e.g. <_example_123 />
                        a.append(b);
                        CALL_MEMBER_FN((RE::BSStringT*)&kThis->str, Replace_MinBufLen)(a.c_str(), 0);
                     }
                     return true;
                  case _traitOpBinaryAnd:
                     kThis->num = (float)((SInt32)kThis->num & (SInt32)argument);
                     return true;
                  case _traitOpBinaryOr:
                     kThis->num = (float)((SInt32)kThis->num | (SInt32)argument);
                     return true;
                  case _traitOpSetIfZero:
                     if (kThis->num == 0.0F)
                        kThis->num = argument;
                     return true;
                  case _traitPrefSave:
                     if (current->isString) {
                        _MESSAGE("XML has asked to save value %f to pref %s.", kThis->num, current->operand.string);
                        //
                        // save the pref.
                        // the save should be carried out when this operator's owning menu is closed.
                        //
                     }
                     return true;
                  case _traitPrefLoad:
                     if (current->isString) {
                        _MESSAGE("XML has asked to load pref %s.", current->operand.string); // TODO: REMOVE LOGGING
                        float result = UIPrefManager::GetInstance().getPrefFloatCurrentValue(current->operand.string);
                        kThis->num = isnan(result) ? 0.0F : result;
                     } else
                        kThis->num = 0.0F;
                     return true;
                  case _traitPrefReset:
                     if (current->isString) {
                        _MESSAGE("XML has asked to reset pref %s. Current working value is %f.", current->operand.string, kThis->num);
                        if (kThis->num = RE::kEntityID_true) {
                           //
                           // TODO
                           //
                           // queue the pref to reset, and return the default value as the result of this operator.
                           // the reset should be carried out when this operator's owning menu is closed.
                           //
                        } else
                           kThis->num = 0.0F;
                     }
                     return true;
               }
               return false;
            };
            __declspec(naked) void Outer() {
               _asm {
                  mov  ecx, dword ptr [esp + 0x14]; // reproduce patched-over instruction // == UInt32 currentOperatorID
                  // // Modded code:
                  mov  eax, dword ptr [esp + 0x50]; // Tile::Value* this
                  mov  edi, dword ptr [esp + 0x44]; // Tile::Value* esp44               // used for the COPY operator
                  mov  edx, dword ptr [esp + 0x1C]; // float esp1C                      // argument for current operator
                  mov  ebp, dword ptr [esp + 0x28]; // Tile::Value::Expression* current // currently being looped on
                  push ecx; // protect value
                  push ebp; // arg5
                  push edx; // arg4
                  push edi; // arg3
                  push eax; // arg2
                  push ecx; // arg1
                  call Inner;
                  add  esp, 0x14; // clean up after args
                  pop  ecx; // restore value
                  //*/ // End of modded code.
                  test al, al;
                  jnz  lHandledCustom;
               lVanilla:
                  cmp  ecx, 0x7D1; // reproduce patched-over CMP
                  mov  eax, 0x0058C348;
                  jmp  eax;
               lHandledCustom:
                  mov  eax, 0x0058C827; // case 0x7EB (pops unused floats from stack and then breaks from the switch-case)
               lExit:
                  jmp  eax;
               };
            };
            void Apply() {
               _MESSAGE("Patching Tile::Value::DoActionEnumeration for our operator hook...");
               HandleMenuQue::Apply();
               WriteRelJump(0x0058C33E, (UInt32)&Outer);
            };
         };
         //
         void Apply() {
            StringOperandSupport::Apply();
            DoActionEnumeration::Apply();
         };
      };
   };
};