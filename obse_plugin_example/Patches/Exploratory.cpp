#include "Exploratory.h"
#include "shared.h"
#include "obse_common/SafeWrite.h"

#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "ReverseEngineered/INISettings.h"

namespace CobbPatches {
   namespace Exploratory {
      namespace Full360MovementTest {
         void _stdcall Inner(RE::Actor* subject, NiVector3& posDelta) {
            constexpr float ce_speedCap = 1.0F;
            //
            if (subject != (RE::Actor*)*g_thePlayer)
               return;
            auto input = RE::OSInputGlobals::GetInstance();
            auto xRaw =  CALL_MEMBER_FN(input, GetJoystickAxisMovement)(0, RE::INI::Controls::iJoystickMoveLeftRight->i);
            auto yRaw = -CALL_MEMBER_FN(input, GetJoystickAxisMovement)(0, RE::INI::Controls::iJoystickMoveFrontBack->i);
            if (xRaw == 0 && yRaw == 0)
               return;
            float x = (float)xRaw * RE::INI::Controls::fJoystickMoveLRMult->f;
            float y = (float)yRaw * RE::INI::Controls::fJoystickMoveFBMult->f;
            //
            /*
            float speed = CALL_MEMBER_FN(subject, GetMovementSpeed)();
            speed *= RE::g_timeInfo->frameTime;
            posDelta.x = speed * x;
            posDelta.y = speed * y;
            */
            x = (std::max)(-ce_speedCap, (std::min)(ce_speedCap, x * 0.02F)); // same multipliers as in the vanilla code circa 0x006720CB
            y = (std::max)(-ce_speedCap, (std::min)(ce_speedCap, y * 0.02F));
            float length = std::sqrt((posDelta.x * posDelta.x) + (posDelta.y * posDelta.y) + (posDelta.z * posDelta.z));
            posDelta.x = length * x;
            posDelta.y = length * y;
            //
            // TODO: Are we sure that posDelta will have consistent lengths? The code that (somehow) eventually 
            //       computes it uses two modifiers which are stored at 0x00B14E5C and 0x00B14E58. The former 
            //       is a "strafe axis magnitude" and is based on the left/right joystick movement only (no 
            //       keyboard effect). The latter is a "main axis magnitude" and is based on the movement of 
            //       whichever joystick is further from the center -- which means that diagonal movement will 
            //       produce shorter values even if the joystick magnitude is the same.
            //
            //       Then again, are those even used? They're stored somewhere, but are they used?
         }
         __declspec(naked) void Outer() {
            _asm {
               lea  ecx, [esp + 0x24]; // reproduce patched-over instruction
               push ecx; // protect
               push edx; // protect
               push ecx;
               push esi;
               call Inner; // _stdcall
               pop  edx; // restore
               pop  ecx; // restore
               push ecx; // reproduce patched-over instruction
               mov  eax, 0x0063CB3A;
               jmp  eax;
            }
         }
         void Apply() {
            WriteRelJump(0x0063CB35, (UInt32)&Outer);
         }
      };
      //
      void Apply() {
         Full360MovementTest::Apply();
      }
   }
}