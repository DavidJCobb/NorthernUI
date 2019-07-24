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
            if (NorthernUI::INI::Features::iEnhancedMovementCameraMode.iCurrent == NorthernUI::kEnhancedMovementCameraType_FreeMovement)
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
      }
      //
      namespace IndependentCamera {
         //
         // TODO: There is an imperfection in our "Skyrim Standard" camera behavior: if you 
         // turn the camera to face a different direction than the player and then move 
         // forward, the camera snaps to the player's direction. We should have the player 
         // snap to the camera's direction on the first frame of movement.
         //
         bool _IsTryingToMove() {
            auto input = RE::OSInputGlobals::GetInstance();
            auto value = CALL_MEMBER_FN(input, GetJoystickAxisMovement)(0, RE::INI::Controls::iJoystickMoveLeftRight->i);
            if (value)
               return true;
            value = CALL_MEMBER_FN(input, GetJoystickAxisMovement)(0, RE::INI::Controls::iJoystickMoveFrontBack->i);
            if (value)
               return true;
            return false;
         }
         bool _ShouldFaceCamera(RE::PlayerCharacter* player) {
            auto mode = NorthernUI::INI::Features::iEnhancedMovementCameraMode.iCurrent;
            if (mode == NorthernUI::kEnhancedMovementCameraType_OblivionStandard)
               return true;
            if (mode == NorthernUI::kEnhancedMovementCameraType_SkyrimStandard)
               if (CALL_MEMBER_FN(player, GetWeaponOut)() || _IsTryingToMove())
                  return true;
            //
            // TODO: Even in "Free Movement" mode, if you have a ranged weapon or spell out, 
            // we should force you to face camera-forward so that you can actually aim and 
            // fire it properly.
            //
            return false;
         }
         //
         namespace ActivatePick {
            //
            // TODO: Make the activation pick (i.e. what your reticle aims at, both for UI 
            // purposes and when activating objects) aim based on the camera yaw rather 
            // than the actor yaw, as in Skyrim.
            //
         }
         namespace CameraRelativeMovement {
            //
            // MobileObject::Move is used to move the player, and it receives a NiVector3 
            // that is intended to be applied relative to the player's facing direction; 
            // that vector represents the player's input (with appropriate scaling for 
            // each animation).
            //
            // We want to make it camera-relative instead. Fortunately, MobileObject::Move 
            // literally just builds a NiMatrix33 from the player yaw, and we can make it 
            // use the camera yaw instead.
            //
            void _stdcall Inner(RE::Actor* subject, NiVector3& delta) {
               if (subject != (RE::Actor*) *g_thePlayer)
                  return;
               if (_ShouldFaceCamera((RE::PlayerCharacter*)subject))
                  return;
               NiVector3 working;
               RE::NiMatrix33 frame;
               CALL_MEMBER_FN(&frame, SetFromYaw)(*RE::fPlayerCameraYaw);
               CALL_MEMBER_FN(&frame, MultiplyByColumn)(working, delta);
               //
               // working = delta shifted from camera-relative coordinates to world coordinates
               //
               CALL_MEMBER_FN(&frame, SetFromYaw)(subject->GetZRotation());
               frame = frame.transpose();
               CALL_MEMBER_FN(&frame, MultiplyByColumn)(delta, working);
               //
               // delta = working shifted from world coordinates to actor-relative coordinates
            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, dword ptr [ebp + 0xC];
                  push eax;
                  push edi;
                  call Inner; // stdcall
                  mov  ecx, edi;
                  mov  eax, 0x0065A2C0; // reproduce patched-over call to MobileObject::GetCharacterProxy
                  call eax;             //
                  mov  ecx, 0x0065AF46;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x0065AF41, (UInt32)&Outer);
            }
         }
         namespace HandlePlayerInput {
            namespace Yaw {
               void _stdcall Inner(RE::PlayerCharacter* player, float yawChange) {
                  if (_ShouldFaceCamera(player))
                     CALL_MEMBER_FN(player, ModifyYaw)(yawChange);
                  else
                     *RE::fPlayerCameraYaw += yawChange;
               }
               __declspec(naked) void Outer() {
                  _asm {
                     mov  ecx, dword ptr [esp];
                     push ecx;
                     push ebx;
                     call Inner; // stdcall
                     add  esp, 4; // cancel patched-over call to MobileObject::ModifyYaw
                     mov  eax, 0x00671D7F;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x00671D7A, (UInt32)&Outer);
               }
            }
            namespace FinalizeCamera {
               void _stdcall Inner(RE::PlayerCharacter* player) {
                  *RE::fPlayerCameraPitch = CALL_MEMBER_FN(player, GetPitch)();
                  if (_ShouldFaceCamera(player))
                     *RE::fPlayerCameraYaw   = player->GetZRotation();
               }
               __declspec(naked) void Outer() {
                  _asm {
                     push ebx;
                     call Inner;
                     mov  eax, 0x00671B34;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x00671B15, (UInt32)&Outer);
               }
            }
            //
            namespace MovementFlags {
               void _stdcall Inner(RE::PlayerCharacter* player, UInt16& flags) {
                  if (_ShouldFaceCamera(player))
                     return;
                  auto input = RE::OSInputGlobals::GetInstance();
                  auto xRaw  =  CALL_MEMBER_FN(input, GetJoystickAxisMovement)(0, RE::INI::Controls::iJoystickMoveLeftRight->i);
                  auto yRaw  = -CALL_MEMBER_FN(input, GetJoystickAxisMovement)(0, RE::INI::Controls::iJoystickMoveFrontBack->i);
                  if (!xRaw && !yRaw) {
                     if (CALL_MEMBER_FN(input, QueryControlState)(RE::kControl_Forward, RE::kKeyQuery_Hold))
                        yRaw += 100;
                     if (CALL_MEMBER_FN(input, QueryControlState)(RE::kControl_Back, RE::kKeyQuery_Hold))
                        yRaw -= 100;
                     if (CALL_MEMBER_FN(input, QueryControlState)(RE::kControl_StrafeRight, RE::kKeyQuery_Hold))
                        xRaw += 100;
                     if (CALL_MEMBER_FN(input, QueryControlState)(RE::kControl_StrafeLeft, RE::kKeyQuery_Hold))
                        xRaw -= 100;
                     if (!xRaw && !yRaw)
                        return;
                  }
                  float angle = (3.14159265358979323846F / 2) - atan2(yRaw, xRaw);
                  player->SetZRotation(*RE::fPlayerCameraYaw + angle);
                  if (flags & 0x000F) {
                     flags = (flags & ~0x003F) | RE::BaseProcess::kMovement_Forward;
                  } else
                     flags = flags & ~0x003F;
               }
               __declspec(naked) void Outer() {
                  _asm {
                     lea  eax, dword ptr [esp + 0x14]; // movement flags to set
                     push eax;
                     push ebx;
                     call Inner; // stdcall
                     mov  ecx, dword ptr [ebx + 0x58]; // reproduce patched-over instructions
                     mov  edx, dword ptr [ecx];        //
                     mov  eax, 0x0067259A;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x00672595, (UInt32)&Outer);
               }
            }
            //
            void Apply() {
               Yaw::Apply();
               FinalizeCamera::Apply();
               MovementFlags::Apply();
            }
         }
         namespace UpdatePlayerCamera {
            namespace NoInitialAssign {
               //
               // Prevent the "camera update" function from snapping the camera angle to the player 
               // when it first runs.
               //
               void _stdcall Inner(RE::PlayerCharacter* player) {
                  *RE::fPlayerCameraPitch = CALL_MEMBER_FN(player, GetPitch)();
                  if (!_ShouldFaceCamera(player))
                     return;
                  *RE::fPlayerCameraYaw   = player->GetZRotation();
               }
               __declspec(naked) void Outer() {
                  _asm {
                     push ebp;
                     call Inner; // stdcall
                     mov  eax, 0x0066B75D;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0066B73F, (UInt32)&Outer);
               }
            }
            namespace SnapForFirstPerson {
               //
               // ...but DO snap it to the player for first-person.
               //
               void _stdcall Inner(RE::PlayerCharacter* player) {
                  if (!_ShouldFaceCamera(player))
                     *RE::fPlayerCameraYaw = player->GetZRotation();
               }
               __declspec(naked) void Outer() {
                  _asm {
                     push ebp;
                     call Inner; // stdcall
                     push ecx;               // reproduce patched-over instructions
                     lea  ecx, [esp + 0x64]; //
                     mov  eax, 0x0066B7BA;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x0066B7B5, (UInt32)&Outer);
               }
            }
            void Apply() {
               NoInitialAssign::Apply();
               SnapForFirstPerson::Apply();
            }
         }
         void Apply() {
            //CameraRelativeMovement::Apply(); // made superfluous by MovementFlags handling, and causes problems if enabled alongside it
            HandlePlayerInput::Apply();
            UpdatePlayerCamera::Apply();
         }
      }
      //
      void Apply() {
         Full360Movement::Apply();
         IndependentCamera::Apply();
      }
   }
}