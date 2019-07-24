#include "EnhancedMovement.h"
#include "shared.h"
#include "obse_common/SafeWrite.h"
#include "obse/NiObjects.h"

#include "Miscellaneous/rotation.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
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
            //
            // We're scaling the movement by the input, but the movement has already been scaled by a multiplier 
            // computed from the input; so, we undo the vanilla scale by simply dividing it instead of adding it.
            //
            posDelta.x = length * x / *RE::fPlayerMoveAnimMult;
            posDelta.y = length * y / *RE::fPlayerMoveAnimMult;
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
                           case RE::kAnimGroup_Left:
                           case RE::kAnimGroup_FastLeft:
                              angle = (3.14159F / 2) - atan2(-x, y);
                              break;
                           case RE::kAnimGroup_Right:
                           case RE::kAnimGroup_FastRight:
                              angle = (3.14159F / 2) - atan2(x, -y);
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