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

#include <cmath>

namespace CobbPatches {
   namespace TagIDs {
      namespace Operators {
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
            bool Inner(const UInt32 operatorID, RE::Tile::Value* const kThis, RE::Tile::Value* const esp44, const float argument, RE::Tile::Value::Expression* const current) {
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
               switch (operatorID) {
                  //
                  // Cases for custom operators should always return true.
                  //
                  // If we wanted to conditionally override a vanilla operator's behavior for some reason, 
                  // we'd return true if we use the custom behavior, or return false to fall through to the 
                  // vanilla behavior.
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
            DoActionEnumeration::Apply();
         };
      };
   };
};