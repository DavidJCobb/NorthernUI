#include "EnhancedMovement.h"
#include "shared.h"
#include "obse_common/SafeWrite.h"
#include "obse/NiObjects.h"

#include "Miscellaneous/rotation.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "ReverseEngineered/INISettings.h"
#include "ReverseEngineered/Miscellaneous.h" // RE::NiMatrix33
#include "Services/INISettings.h"

NiNode* _GetNodeByName(NiNode* root, const char* name) {
   if (root->m_pcName && _stricmp(root->m_pcName, name) == 0)
      return root;
   for (UInt32 i = 0; i < root->m_children.firstFreeEntry; i++) {
      auto child = root->m_children.data[i];
      if (!child)
         continue;
      NiNode* casted = (NiNode*) child->Unk_02();
      if (!casted)
         continue;
      if (casted->m_pcName && _stricmp(casted->m_pcName, name) == 0)
         return casted;
   }
   return nullptr;
}

namespace CobbPatches {
   namespace EnhancedMovement {
      namespace Full360Movement {
         constexpr float ce_speedCap   = 1.0F;
         constexpr char* ce_targetNode = "Bip01";
         //
         void _stdcall Inner(RE::Actor* subject, NiVector3& posDelta) {
            if (!NorthernUI::INI::Features::bEnhancedMovement360Movement.bCurrent)
               return;
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
            //
            {  // Facing direction
               //
               // TODO: Once we've decided on all the different behaviors we want for animations, add an INI 
               //       setting that controls them via an enum
               //
               auto root = subject->niNode;
               if (root) {
                  auto node = _GetNodeByName(root, ce_targetNode);
                  if (node) {
                     float angle = 0.0F;
                     auto  animData = (RE::ActorAnimData*) subject->GetAnimData();
                     if (animData) {
                        //RE::AnimCode code = CALL_MEMBER_FN(animData, GetCurrentAnimCodeForBodyPart)(RE::kAnimBodyPart_LowerBody);
                        RE::AnimCode code = animData->GetCurrentAnimCodeForBodyPart(RE::kAnimBodyPart_LowerBody);
                        switch (code.type()) {
                           case RE::kAnimGroup_Forward:
                           case RE::kAnimGroup_FastForward:
                              angle = (3.14159F / 2) - atan2(y, x);
                              break;
                           case RE::kAnimGroup_Backward:
                           case RE::kAnimGroup_FastBackward:
                              angle = (3.14159F / 2) - atan2(-y, -x);
                              break;
                           default:
                              break;
                        }
                     }
                     CALL_MEMBER_FN((RE::NiMatrix33*)&node->m_localRotate, SetFromYaw)(angle);
                  }
               }
            }
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
         Full360Movement::Apply();
      }
   }
}