#include "./Main.h"
#include "./Operators.h"
#include "obse_common/SafeWrite.h"
#include "shared.h"

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
            bool Inner(const UInt32 operatorID, RE::Tile::Value* const kThis, RE::Tile::Value* const esp44, const float argument, RE::Tile::Value::Expression* const current) {
               //
               // This subroutine should generally only handle our custom operators, since we just 
               // jump back to the vanilla code for vanilla operators.
               //
               if (g_detectedMenuQue && g_detectedMenuQueBaseAddress) {
                  typedef void(__stdcall *f_MQ_012570)(decltype(kThis), decltype(esp44), decltype(current), UInt32 operatorID);
                  f_MQ_012570 MQ_012570 = (f_MQ_012570)(g_detectedMenuQueBaseAddress + 0x00012570);
                  //
                  // This call always has to be made, even if we're not executing a MenuQue operator. 
                  // I suspect it plays a role in managing MenuQue's "string stack" or somesuch.
                  //
                  MQ_012570(kThis, esp44, current, operatorID);
                  if (operatorID == RE::kTagID_copy || (operatorID >= 0x7EC && operatorID <= 0x7F0))
                     return true;
               }
               switch (operatorID) {
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