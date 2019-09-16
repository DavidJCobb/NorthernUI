#include "./Main.h"
#include "./Operators.h"
#include "obse_common/SafeWrite.h"
#include "shared.h"

#include "Fun/x86Reader.h"
#include "Miscellaneous/math.h"
#include "Miscellaneous/strings.h"
#include "ReverseEngineered/NetImmerse/NiTypes.h"
#include "ReverseEngineered/UI/Menu.h"
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
            namespace UpdateInboundReferences {
               //
               // When a trait gets updated and fully recomputes its value, it 
               // calls Tile::Value::UpdateInboundReferences, which does two 
               // things:
               //
               //  - First, the trait's new value is propagated to all operators 
               //    that pull from it via the SRC and TRAIT attributes.
               //
               //  - Then, all such operators are found and looped over: their 
               //    containing traits are told to recompute their own values 
               //    via a call to Tile::Value::DoActionEnumeration(0).
               //
               // That first step is very important.
               //
               //  - If the changed trait has a float value, then all incoming 
               //    operators have their operands set to that float.
               //
               //  - However, if the chaned trait has a string value, then all 
               //    incoming operators have their containing trait's string 
               //    value set to the new string (if it's changed).
               //
               // That means that in the vanilla engine, situations like this 
               // will behave undesirably:
               //
               //    <rect name="test">
               //       <foo> String! </foo>
               //       <bar> <copy src="me()" trait="foo" /> </bar>
               //       <baz>
               //          <someOperatorWithAStringOperandThatReturnsAFloat src="me()" trait="bar" />
               //       </baz>
               //    </rect>
               //
               // Even though someOperatorWithAStringOperandThatReturnsAFloat 
               // returns a float, it's still pulling a string value from the 
               // bar trait. Therefore, when the bar trait is updated and gains 
               // a string value, UpdateInboundReferences will directly set the 
               // value of the baz trait to that string. This is the behavior we 
               // want to change.
               //
               // For the record: MenuQue doesn't account for this case, which 
               // would in theory potentially break its <length /> operator.
               //
               bool _stdcall DoString(RE::Tile::Value::Expression* referring, const char* newString) {
                  auto opcode = referring->opcode;
                  if (CobbPatches::TagIDs::IsOperatorWithStringOperand(opcode)) {
                     //
                     // Propagate the new computed string to the operator's operand, just as 
                     // the vanilla game would a float operand.
                     //
                     if (referring->isString && referring->operand.string) {
                        FormHeap_Free((void*)referring->operand.string);
                     }
                     auto length = strlen(newString) + 1;
                     auto buffer = (char*)FormHeap_Allocate(length);
                     memcpy(buffer, newString, length - 1);
                     buffer[length - 1] = '\0';
                     referring->operand.string = buffer;
                     referring->isString = true;
                     return true;
                  }
                  if (opcode == 0) // make sure that killed-at-runtime operators can't damage their containing traits
                     return true;
                  return false; // return false to use vanilla handling
               }
               __declspec(naked) void Outer() {
                  _asm {
                     push ecx; // protect
                     push edx;
                     push ebx;
                     call DoString; // stdcall
                     pop  ecx; // restore
                     test al, al;
                     jnz  lHandled;
                     mov  eax, 0x004028D0; // reproduce patchedover call to BSStringT::Replace_MinBufLen(const char*, UInt32)
                     call eax;             // 
                     jmp  lExit;
                  lHandled:
                     add  esp, 8; // cancel patched-over call
                  lExit:
                     mov  eax, 0x0058BE4E;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0058BE49, (UInt32)&Outer);
               }
            }
            //
            void Apply() {
               ExpressionDestructor::Apply();
               ExpressionRemove::Apply();
               UpdateTemplatedChildren::Apply();
               UpdateInboundReferences::Apply();
            }
         };
         namespace DoActionEnumerationTracking {
            //
            // MenuQue plugs a lot of static variables into DoActionEnumeration, 
            // which means that we can't allow operators to take any action that 
            // might change another trait's value; doing so causes MenuQue to 
            // choke and crash the game. (Changing one trait while we're process-
            // ing changes for another trait causes DoActionEnumeration to recurse 
            // in a way that breaks MenuQue's state -- things get deleted when 
            // they're still needed.)
            //
            // As such, we need to be able to track when we're no longer inside 
            // of DoActionEnumeration -- bearing in mind that it can recurse 
            // indirectly due to UpdateInboundReferences.
            //
            // The (s_depth) variable keeps track of how many times we've ended 
            // up in DoActionEnumeration i.e. how far we've recursed, with 0 
            // indicating that we're outside of the "meat" of the function and 
            // it's safe to recurse. The (s_handlingEnd) function prevents us 
            // from recursing when we actually act on DoActionEnumeration being 
            // over: when the function is over, we'll be taking actions that 
            // alter trait values, which will make the function recurse; we need 
            // to ensure that we don't react to *that* call stack ending.
            //
            static UInt32 s_depth = 0;
            static bool   s_handlingEnd = false;
            namespace Start {
               void Inner() {
                  s_depth++;
               }
               __declspec(naked) void Outer() {
                  _asm {
                     lea  ebx, [esi + 4];         // reproduce patched-over instruction
                     fstp dword ptr [esp + 0x48]; // reproduce patched-over instruction
                     call Inner;
                     mov  eax, 0x0058BF7E;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0058BF77, (UInt32)&Outer);
                  SafeWrite16(0x0058BF77 + 5, 0x9090); // courtesy NOPs
               }
            }
            namespace End {
               void Inner() {
                  if (!s_depth) {
                     _MESSAGE("WARNING: DoActionEnumeration call tracking has gone wrong!");
                     return;
                  }
                  s_depth--;
                  if (!s_depth) {
                     if (s_handlingEnd)
                        return;
                     s_handlingEnd = true; // pushQueuedPrefsToUIState changes trait values and can cause us to effectively recurse
                     UIPrefManager::GetInstance().pushQueuedPrefsToUIState();
                     s_handlingEnd = false;
                  }
               }
               __declspec(naked) void Outer() {
                  _asm {
                     mov  eax, 0x00589690; // reproduce patched-over call
                     call eax;             //
                     call Inner;
                     mov  eax, 0x0058C964;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0058C95F, (UInt32)&Outer);
               }
            }
            //
            void Apply() {
               Start::Apply();
               End::Apply();
            }
         }
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
            namespace {
               namespace Helpers {
                  const char* getStringOperandAsPrefName(RE::Tile::Value::Expression* const op) {
                     const char* str = op->GetStringValue();
                     if (!str || !cobb::string_has_content(str))
                        return nullptr;
                     if (str[0] == '_') {
                        str++;
                        if (!str[0])
                           return nullptr;
                     }
                     return str;
                  }
                  UInt32 getValueContainingMenuID(RE::Tile::Value* const value) {
                     auto tile = value->owner;
                     if (tile) {
                        auto menu = CALL_MEMBER_FN(tile, GetContainingMenu)();
                        if (menu)
                           return menu->GetID();
                     }
                     return 0;
                  }
               }
            }
            //
            static float s_opState_prefLoopMin = 0.0F;
            static float s_opState_prefLoopMax = 0.0F;
            bool Inner(const UInt16 operatorID, RE::Tile::Value* const kThis, RE::Tile::Value* const esp44, const float argument, RE::Tile::Value::Expression* const current) {
               //
               // This subroutine should generally only handle our custom operators, since we just 
               // jump back to the vanilla code for vanilla operators.
               //
               if (HandleMenuQue::mqOperatorHandler) { // MQ_12570 in v16b
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
                  // OPERATORS CANNOT TAKE ACTIONS THAT IMMEDIATELY LEAD TO THE CHANGING OF ANY OTHER TRAIT. 
                  // MenuQue patches in a ton of static vars into DoActionEnumeration, and it WILL CRASH if 
                  // you try to change one trait while another trait is already changing!
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
                  case _traitOpPrefModify:
                     {
                        const char* str = Helpers::getStringOperandAsPrefName(current);
                        if (!str)
                           //
                           // Expect this to happen sometimes. If you're using this operator, say, 
                           // at the end of a scrollbar thumb's _value_scrolled_to, then when the 
                           // scrollbar is first instantiated, all of the prior outgoing refs from 
                           // that trait will cause the trait to be recomputed, so this operator 
                           // will be called with a nullptr string several times in a row until 
                           // the operator itself is resolved.
                           //
                           // e.g.
                           //
                           //    <someTrait>
                           //       <copy src="foo" trait="bar" />
                           //       <xxnPrefSaveValue src="foo" trait="baz" />
                           //    </someTrait>
                           //
                           // will cause (someTrait) to be recomputed twice: once to get the value 
                           // of the copy operator, and once to get the value of the pref-save 
                           // operator; and during that first recomputation, the pref-save operator 
                           // will have a nullptr string because its string hasn't been set up yet.
                           //
                           return true;
                        if (!kThis->num) // skip changes-by-zero
                           return true;
                        UInt32 menuID = Helpers::getValueContainingMenuID(kThis);
                        UIPrefManager::GetInstance().modifyPrefValue(str, kThis->num, menuID);
                     }
                     return true;
                  case _traitOpPrefClampToMin:
                     {
                        const char* str = Helpers::getStringOperandAsPrefName(current);
                        if (!str)
                           return true;
                        UInt32 menuID = Helpers::getValueContainingMenuID(kThis);
                        UIPrefManager::GetInstance().clampPrefValue(str, kThis->num, true, menuID);
                     }
                     return true;
                  case _traitOpPrefClampToMax:
                     {
                        const char* str = Helpers::getStringOperandAsPrefName(current);
                        if (!str)
                           return true;
                        UInt32 menuID = Helpers::getValueContainingMenuID(kThis);
                        UIPrefManager::GetInstance().clampPrefValue(str, kThis->num, false, menuID);
                     }
                     return true;
                  case _traitOpPrefReset:
                     {
                        const char* str = Helpers::getStringOperandAsPrefName(current);
                        if (!str)
                           return true;
                        if (kThis->num == (float)RE::kEntityID_true) {
                           UInt32 menuID = Helpers::getValueContainingMenuID(kThis);
                           UIPrefManager::GetInstance().resetPrefValue(str, menuID);
                        }
                     }
                     return true;
                  case _traitOpPrefModulo:
                     {
                        if (!kThis->num)
                           return true;
                        const char* str = Helpers::getStringOperandAsPrefName(current);
                        if (!str)
                           return true;
                        UInt32 menuID  = Helpers::getValueContainingMenuID(kThis);
                        auto&  manager = UIPrefManager::GetInstance();
                        SInt32 value   = (SInt32)manager.getPrefCurrentValue(str, menuID) % (SInt32)kThis->num;
                        manager.setPrefValue(str, value, menuID);
                     }
                     return true;
                  case _traitOpPrefCarousel:
                     //
                     // Same as modulo, except that we never write a zero value. In other words:
                     // 
                     //  - If the pref == the current working value, we make no changes.
                     //
                     //  - If the pref == 0, then we set it to the current working value.
                     //
                     //  - Otherwise, we set the pref to its value modulo the current working value.
                     //
                     // Consider:
                     //
                     //    <copy src="me()" trait="clicked" />
                     //    <xxnOpPrefModifyValue>PrefName</xxnOpPrefModifyValue>
                     //    <copy>2</copy>
                     //    <OPERATOR_HERE>PrefName</OPERATOR_HERE>
                     //
                     // For modulo, given an initial value of 2, you'd see this sequence: 2, 1, 0, 1, 0
                     // For carousel, you'd see this sequence: 2, 1, 2, 1, 2
                     //
                     {
                        if (!kThis->num)
                           return true;
                        const char* str = Helpers::getStringOperandAsPrefName(current);
                        if (!str)
                           return true;
                        auto&  manager = UIPrefManager::GetInstance();
                        UInt32 menuID  = Helpers::getValueContainingMenuID(kThis);
                        SInt32 value   = manager.getPrefCurrentValue(str, menuID);
                        SInt32 operand = kThis->num;
                        if (value <= 0)
                           manager.setPrefValue(str, operand, menuID);
                        else if (value != operand)
                           manager.setPrefValue(str, (value % operand), menuID);
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
            DoActionEnumerationTracking::Apply();
         };
      };
   };
};