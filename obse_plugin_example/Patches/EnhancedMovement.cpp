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

#include "NewMenus/XXNHUDDebugMenu.h"

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
            if (NorthernUI::INI::Features::iEnhancedMovementCameraMode.iCurrent == NorthernUI::kEnhancedMovementCameraType_FreeMovement) {
               //
               // When we're in third-person view, the "free" camera handles movement by always moving the 
               // player-actor forward and just changing the actor's facing direction.
               //
               auto* player = (RE::PlayerCharacter*)*g_thePlayer;
               if (player->isThirdPerson && !player->IsOblivionReloadedImmersiveFirstPerson())
                  return;
            }
            if (subject->IsKnockedDown()) // also checks for death
               //
               // The calculations below malfunction horribly whenever the player is ragdolling, including 
               // when they're dead, for unknown reasons. This malfunction causes the player to rocket off 
               // a significant distance when trying to move. In outdoor environments, this triggers a load 
               // screen that never completes. This is not a crash -- if you use memory hacking to set 
               // OSGlobals::quitToMainMenuQueued to true, then the game immediately returns to the main 
               // menu, though I can't speak as to any side-effects -- but it's obviously undesirable.
               // 
               // It's especially nasty for death because whenever the player dies, the game forces them 
               // to carry out a zero-length movement, which immediately launches them and softlocks the 
               // game.
               //
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
            { // properly cap diagonals, too
               float l = std::sqrt((x * x) + (y * y));
               if (l > ce_speedCap) {
                  x = x / l * ce_speedCap;
                  y = y / l * ce_speedCap;
               }
            }
            float length = std::sqrt((posDelta.x * posDelta.x) + (posDelta.y * posDelta.y) + (posDelta.z * posDelta.z));
            //
            // We're scaling the movement by the input, but the movement has already been scaled by a multiplier 
            // computed from the input; so, we undo the vanilla scale by simply dividing it instead of adding it.
            //
            posDelta.x = length * x / *RE::fPlayerMoveAnimMult;
            posDelta.y = length * y / *RE::fPlayerMoveAnimMult;
            //
            {  // Facing direction
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
            WriteRelJump(0x0063CB35, (UInt32)&Outer); // HighProcess::Move+0x405
         }
      }
      //
      namespace IndependentCamera {
         //
         // This patch allows the user to choose between three different behaviors for 
         // the third-person camera:
         //
         // OBLIVION STANDARD: The camera and the player always face in the same direct-
         // ion. The "look" inputs rotate the player-character, and the camera is kept 
         // in synch with her.
         //
         // SKYRIM STANDARD: If the player-character isn't moving and doesn't have her 
         // weapon drawn, then the camera can be rotated independently of her. If the 
         // camera is facing in a different direction from her when the player starts 
         // moving, then she snaps to the camera at the start of the movement.
         //
         // FREE MOVEMENT: The camera and the player-character can face different 
         // directions even during movement or with a melee weapon drawn; the player-
         // character is rotated to face in her direction of movement. However, if a 
         // ranged weapon or spell is equipped, then she is forced to face camera-forward 
         // so that aiming works properly.
         //
         // -----------------------------------------------------------------------------
         //
         // We also adjust the inertial effect that Oblivion applies to the camera, 
         // offering the player three options:
         //
         // STANDARD: The camera inertia effect is not modified in any way.
         //
         // FIXED: We fix a bug in the camera inertia effect that can cause the camera 
         // to swing wildly when you pass through some load doors, depending on the 
         // difference between your angle going into the first door and your angle 
         // coming out of the second door. Because the effect generally only happens 
         // as you walk in and out of buildings, it almost looks like an intentional 
         // attempt at showing off the environment you're stepping into; however, it 
         // messes with movement and in practice, it's disorienting and irritating 
         // beyond measure.
         //
         // DISABLE: Disable the camera inertia entirely.
         //
         // -----------------------------------------------------------------------------
         //
         // TODO: Every time the player switches to first-person, the player should be 
         // rotated to match the camera. (Be careful: the input handler switches the 
         // player in and out of first-person view repeatedly for some reason!) Unless... 
         // What exactly does Skyrim do, again?
         //
         NorthernUI::EnhancedMovementCameraType _GetPref() {
            auto mode = NorthernUI::INI::Features::iEnhancedMovementCameraMode.iCurrent;
            switch (mode) {
               case NorthernUI::kEnhancedMovementCameraType_FreeMovement:
               case NorthernUI::kEnhancedMovementCameraType_OblivionStandard:
               case NorthernUI::kEnhancedMovementCameraType_SkyrimStandard:
                  return (NorthernUI::EnhancedMovementCameraType)mode;
            }
            return NorthernUI::kEnhancedMovementCameraType_OblivionStandard;
         }
         //
         // These static bools exist to facilitate the "Skyrim Standard" camera behavior. 
         // If you turn the camera to face a different direction from your character and 
         // then start moving, your character should snap to face the camera (instead of 
         // the camera snapping to face the player again). As such, we need to be able to 
         // tell whether the current frame is the first frame of your movement.
         //
         static bool   s_lastFrameMovement = false;
         static bool   s_thisFrameMovement = false;
         static UInt32 s_lastWorldOrCellID = 0;
         static UInt32 s_thisWorldOrCellID = 0;
         bool _JustStartedMoving() {
            return s_thisFrameMovement && !s_lastFrameMovement;
         }
         bool _WorldOrCellChanged() {
            return s_thisWorldOrCellID != s_lastWorldOrCellID;
         }
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
         bool _ShouldFaceCamera(RE::PlayerCharacter* player) { // Should player-forward and camera-forward synch?
            //
            // Running these checks multiple times a frame is messy, but unless I just go 
            // all out and replace the entire player input handler at 00671620, this is 
            // the only way to do it.
            //
            if (*RE::bPlayerInVanityMode) // you can move while in Vanity Mode, so since we haven't figured out the best way to replace Vanity Mode, let's just back off so that it behaves sensibly, yeah?
               return true;
            if (!player->isThirdPerson)
               return true;
            if (player->IsOblivionReloadedImmersiveFirstPerson()) // Oblivion Reloaded's immersive first-person view is actually third-person with the camera stuck in the player's head; if we tamper with the player model, we break that
               return true;
            if (_WorldOrCellChanged())
               return true;
            if (player->GetMountedHorse()) // I don't understand mounted camera control well enough to tamper with it; the patches I have for control on foot break it
               return true;
            if (CALL_MEMBER_FN(player, IsInDialogue)())
               return true;
            auto mode = _GetPref();
            if (mode == NorthernUI::kEnhancedMovementCameraType_OblivionStandard)
               return true;
            if (mode == NorthernUI::kEnhancedMovementCameraType_SkyrimStandard) {
               if (CALL_MEMBER_FN(player, GetWeaponOut)() || _IsTryingToMove())
                  return true;
            }
            if (mode == NorthernUI::kEnhancedMovementCameraType_FreeMovement) {
               //
               // If the player has their weapon/spell drawn, and the weapon or the spell 
               // is ranged, then force the player to face forward so that they can aim 
               // and fire it properly.
               //
               if (CALL_MEMBER_FN(player, GetWeaponOut)()) {
                  //
                  // Check weapon...
                  //
                  auto process = player->process;
                  if (process) {
                     auto weapon = process->GetEquippedWeaponData(true);
                     if (weapon) {
                        auto form = (TESObjectWEAP*)weapon->type;
                        if (form->type == TESObjectWEAP::kType_Bow || form->type == TESObjectWEAP::kType_Staff) {
                           return true;
                        }
                     }
                  }
                  //
                  // Check spell...
                  //
                  auto spell = CALL_MEMBER_FN(player, GetEquippedMagicItem)();
                  if (spell) {
                     auto node = &spell->list.effectList;
                     do {
                        auto effect = node->effectItem;
                        if (effect->range == EffectItem::kRange_Target)
                           return true;
                     } while (node = node->next);
                  }
               }
            }
            return false;
         }
         void _UpdateFrameMovementState() {
            s_lastFrameMovement = s_thisFrameMovement;
            s_thisFrameMovement = _IsTryingToMove();
            //
            s_lastWorldOrCellID = s_thisWorldOrCellID;
            auto cell = (*g_thePlayer)->parentCell;
            if (cell) {
               auto world = cell->worldSpace;
               if (world)
                  s_thisWorldOrCellID = world->refID;
               else
                  s_thisWorldOrCellID = cell->refID;
            } else
               s_thisWorldOrCellID = 0;
         }
         //
         namespace ActivatePick {
            //
            // When using the "Skyrim" or "Free" camera modes, this code makes the reticle 
            // aim camera-forward rather than actor-forward. This means that you can activate 
            // objects even if your character is facing away from them, but activating the 
            // thing YOU'RE looking at is smoother and more intuitive than activating the 
            // thing that YOUR CHARACTER is looking at -- especially when you factor in 
            // camera pitch and small objects bunched together (think of things on shelves).
            //
            float _stdcall Inner() {
               auto player = (RE::PlayerCharacter*) *g_thePlayer;
               if (_ShouldFaceCamera(player))
                  return player->GetZRotation();
               return *RE::fPlayerCameraYaw;
            }
            __declspec(naked) void Outer() {
               _asm {
                  fstp st(0); // replace this with Inner's return value
                  call Inner; // stdcall
                  push ecx;               // reproduce patched-over instructions
                  lea  ecx, [esp + 0x60]; //
                  mov  eax, 0x005807AB;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x005807A6, (UInt32)&Outer); // InterfaceManager::MaintainReticleTarget+0xD6
            }
         }
         namespace Compass {
            //
            // By default, the compass uses the player's rotation rather than the 
            // camera's rotation.
            //
            float _stdcall Inner(RE::PlayerCharacter* player) {
               if (_GetPref() == NorthernUI::kEnhancedMovementCameraType_OblivionStandard)
                  return player->GetZRotation();
               return *RE::fPlayerCameraYaw;
            }
            __declspec(naked) void Outer() {
               _asm {
                  push ecx;
                  call Inner; // stdcall
                  mov  eax, 0x005A6F15;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x005A6F0B, (UInt32)&Outer); // somewhere in HUDMainMenu::HandleFrame
               SafeWrite16 (0x005A6F10, 0x9090); // courtesy NOPs
               SafeWrite8  (0x005A6F12, 0x90);
            }
         }
         namespace HandlePlayerInput {
            //
            // Patches to the subroutine that handles most player input.
            //
            namespace PrepCamera {
               //
               // We need to reset the camera to match the player on the frame that the player 
               // loads into a new area.
               //
               // We maintain the form ID of the cell or worldspace the player was in on the 
               // previous frame, and compare to that every frame.
               //
               void _stdcall Inner(RE::PlayerCharacter* player) {
                  _UpdateFrameMovementState();
                  *RE::fPlayerCameraPitch = CALL_MEMBER_FN(player, GetPitch)();
                  if (_ShouldFaceCamera(player)) {
                     if (_GetPref() == NorthernUI::kEnhancedMovementCameraType_SkyrimStandard) {
                        if (_JustStartedMoving())
                           player->SetZRotation(*RE::fPlayerCameraYaw);
                     }
                     *RE::fPlayerCameraYaw = player->GetZRotation();
                  }
                  switch (NorthernUI::INI::Features::iCameraInertiaMode.iCurrent) {
                     case NorthernUI::kCameraInertiaMode_Fixed:
                        if (s_lastWorldOrCellID == s_thisWorldOrCellID)
                           break;
                     case NorthernUI::kCameraInertiaMode_Disabled:
                        *RE::bCameraInertiaResetQueued = true;
                        break;
                  }
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
                  SafeWrite16 (0x00671B1A, 0x9090); // courtesy NOPs
                  SafeWrite8  (0x00671B1C, 0x90);
               }
            }
            namespace Yaw {
               //
               // Intercept the code that takes the player's look input and applies it. (We only 
               // need to intercept the yaw angle; we should keep pitch synched for the player 
               // and camera.)
               //
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
            //
            namespace MovementFlags {
               //
               // Intercept the call to PlayerCharacter::process::SetMovementFlags, so that we 
               // can tamper with the movement flags to set and the player's yaw angle as needed 
               // for the current camera mode.
               //
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
               PrepCamera::Apply();
               Yaw::Apply();
               MovementFlags::Apply();
            }
         }
         namespace POVSwitch {
            //
            // When the player switches to first-person view, snap them to face camera-forward 
            // in order to maintain visual continuity.
            //
            // TODO: this doesn't seem to work
            //
            void _stdcall Inner(bool isThirdPerson) {
               if (isThirdPerson)
                  return;
               if (_GetPref() == NorthernUI::kEnhancedMovementCameraType_OblivionStandard)
                  return;
               ((RE::PlayerCharacter*)*g_thePlayer)->SetZRotation(*RE::fPlayerCameraYaw);
            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  byte ptr [esi + 0x588], al; // reproduce patched-over instruction
                  push eax; // protect
                  push al;
                  call Inner; // stdcall
                  pop  eax; // restore
                  test al, al; // reproduce overridden comparison
                  mov  ecx, 0x0066C59E;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x0066C598, (UInt32)&Outer);
               SafeWrite8  (0x0066C59D, 0x90); // courtesy NOP
            }
         }
         namespace UpdatePlayerCamera {
            //
            // Patches to PlayerCharacter::UpdateCamera.
            //
            namespace NoInitialAssign {
               //
               // Prevent the "camera update" function from snapping the camera angle to the player 
               // when it first runs.
               //
               void _stdcall Inner(RE::PlayerCharacter* player) {
                  *RE::fPlayerCameraPitch = CALL_MEMBER_FN(player, GetPitch)();
                  if (!_ShouldFaceCamera(player))
                     return;
                  *RE::fPlayerCameraYaw = player->GetZRotation();
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
                  SafeWrite32 (0x0066B744, 0x90909090); // courtesy NOPs
               }
            }
            void Apply() {
               NoInitialAssign::Apply();
            }
         }
         void Apply() {
            HandlePlayerInput::Apply();
            UpdatePlayerCamera::Apply();
            //POVSwitch::Apply();
            //
            ActivatePick::Apply();
            Compass::Apply();
         }
      }
      //
      void Apply() {
         Full360Movement::Apply();
         IndependentCamera::Apply();
      }
   }
}