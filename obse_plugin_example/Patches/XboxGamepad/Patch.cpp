#include "Patch.h"
#include "Main.h"
#include "CustomControls.h"
#include "Services/INISettings.h"
#include "Services/PatchManagement.h"
#include "Patches/NewMenus/XXNHUDDebugMenu.h"
#include "shared.h" // SafeMemset

#include "ReverseEngineered/INISettings.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/GameSettings.h"
#include "ReverseEngineered/NetImmerse/NiObject.h"
#include "ReverseEngineered/NetImmerse/NiProperty.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/Systems/Sound.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/LockPickMenu.h"
#include "ReverseEngineered/UI/Menus/MapMenu.h"
#include "obse/NiTypes.h"
#include "obse_common/SafeWrite.h"

#include <algorithm>
using namespace std;
#include "Miscellaneous/math.h"

#include "obse/GameMenus.h" // debug

// If this is set to 1, then we won't listen for the Start button (to pause the game) 
// during gameplay, and will have the UI system handle it when in menu mode. In practice, 
// that's not really what we want; it prevents you from pausing outside of a menu, and 
// while it does allow the xbuttonstart handler to work, that handler doesn't actually 
// prevent the game from popping up the PauseMenu.
//
#define XXN_USE_UI_SIDE_PAUSE 0

namespace CobbPatches {
   namespace XboxGamepad {
      extern bool g_uiCursorGamepadControlEnabled = false; // see UICursorGamepadControl

      static bool s_savingJoystickUsesNativeDefaults = false; // used for the retroactive fix; see LoadControls::RetroactiveINIFix

      namespace FixMovementZeroing {
         //
         // For some reason, Bethesda used a static float (fPlayerMoveAnimMult) to 
         // keep track of the gamepad's influence on player movement. The float is 
         // a multiplier by which the player's movement and animation speed will be 
         // scaled. It initializes to 1.0F and is only modified if gamepad input is 
         // received, so if you don't play with a gamepad at all, then the float 
         // stays at 1.0F and all WASD movement happens at full speed.
         //
         // However, if you move the joystick with a gamepad, then the float will 
         // change to reflect the joystick magnitude (I'm simplifying). The problem 
         // is this: when you return the joystick to a neutral (zero) position, you 
         // are by definition no longer moving the joystick... which means that the 
         // float doesn't reset; it retains the value from the last frame that the 
         // joystick was off-center, and this value will reflect an extremely small 
         // joystick magnitude. This means that from that point on, all WASD move-
         // ment will be carried out using an extremely small multiplier, slowing 
         // the player to a crawl.
         //
         // This patch resets fPlayerMoveAnimMult to 1.0F whenever the joystick is 
         // not being used.
         //
         // A few notes:
         //
         //  - Why did they use a static float? It's only used by one function, and 
         //    its value doesn't need to be retained between calls. They could've 
         //    just used a local variable, and manually set it to 1.0F when there 
         //    was no gamepad input.
         //
         //  - The float isn't actually based on the joystick magnitude; it's 
         //    based on the magnitude of the furthest-moved joystick axis. This is 
         //    an important distinction: if you have the joystick at a perfect 
         //    diagonal, then both axes are going to be (+/-)70% of the maximum 
         //    range... which means that fPlayerMoveAnimMult will be about 0.7. In 
         //    other words, in the vanilla game, it's impossible to move at full 
         //    speed at a diagonal angle using a joystick. (Our EnhancedMovement.h 
         //    patches address this.)
         //
         //    The PC build has noticeably incomplete gamepad support compared to 
         //    the Xbox build (as one might infer from the sheer size of this 
         //    file). Hard to say whether the speed issue affects the Xbox build.
         //
         static_assert(RE::fPlayerMoveAnimMult == (float*)0x00B14E58, "Movement zeroing fix: fPlayerMoveAnimMult is in a different place.");
         __declspec(naked) void Outer() {
            _asm {
               // vanilla: test eax, eax;
               jz   lZero;
               jge  lPositive;
               push 0; // reproduce patched-over instruction
               mov  ecx, 0x00672166;
               jmp  ecx;
            lPositive:
               mov  ecx, 0x00672168;
               jmp  ecx;
            lZero:
               mov  eax, 0x00B14E58; // fPlayerMoveAnimMult
               mov  dword ptr [eax], 0x3F800000;
               mov  eax, 0x0067218B;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x00672160, (UInt32)&Outer);
            SafeWrite8  (0x00672165, 0x90); // courtesy NOP
         };
      };
      namespace PauseKey {
         __declspec(naked) void Outer() {
            _asm {
               mov  cl, 0x80;
               xor  eax, eax;
               test byte ptr [edi + 0x19F5], cl; // DIK_ESC, last frame
               jnz  lNotKeyboard;
               test byte ptr [edi + 0x18F5], cl; // DIK_ESC, this frame
               jz   lNotKeyboard;
               mov  al, 1;
               jmp  lExit;
            lNotKeyboard:
               xor  edx, edx;                           // UInt32 i = 0;
            lLoopStart:                                 // do {
               call XXNGamepadSupportCore::GetInstance; //
               mov  ecx, eax;                           //
               push edx;                                //
               call XXNGamepadSupportCore::GetGamepad;  //    eax = XXNGamepadSupportCore::GetInstance()->GetGamepad(i);
               inc  edx;                                //    i++;
               test eax, eax;                           //    if (!eax)
               je   lLoopStart;                         //       continue;
               push edx;                                // // edx is considered a temporary register; we need to protect it
               push 2;                                  //
               push 4;                                  // // XXNGamepad::kGamepadButton_Start
               mov  ecx, eax;                           //
               call XXNGamepad::GetButtonState;         //       eax = eax->GetButtonState(XXNGamepad::kGamepadButton_Start, 2);
               pop  edx;                                // // restore protected edx
               test eax, eax;                           //       if (eax)
               jnz  lExit;                              //          return eax;
               cmp  edx, 4;                             //
               jl   lLoopStart;                         // } while (i < 4);
            lExit:
               pop edi;
               retn 8;
            };
         };
         void Apply() {
            #if XXN_USE_UI_SIDE_PAUSE == 1
               return;
            #endif
            WriteRelJump(0x004035A8, (UInt32)&Outer);
            SafeWrite32 (0x004035AD, 0x90909090); // courtesy NOPs
            SafeWrite16 (0x004035B1, 0x9090);
            SafeWrite8  (0x004035B3, 0x90);
         };
      };
      namespace LoadControls {
         namespace RetroactiveINIFix {
            //
            // Retroactively fix any settings we broke before we implemented the "DontSaveOurMappings" 
            // patch. Note that when this hook runs, the OSInputGlobals object is still under constr-
            // uction, so it's not in its usual pointer yet.
            //
            bool IsBroken(volatile RE::OSInputGlobals* instance) {
               auto& config  = XXNGamepadConfigManager::GetInstance();
               auto  profile = config.GetProfileOrDefault(config.currentScheme);
               if (!profile)
                  profile = &config.defaultProfile;
               auto& maps = instance->joystick;
               for (int i = 0; i < std::extent<decltype(RE::g_defaultJoystickMappings)>::value; i++) {
                  if (RE::g_defaultJoystickMappings[i] == 0xFF)
                     continue;
                  if (maps.bindings[i] != profile->bindings[i])
                     return false;
               }
               return true;
            };
            void Execute(RE::OSInputGlobals* instance) {
               if (IsBroken(instance)) {
                  s_savingJoystickUsesNativeDefaults = true;
                  CALL_MEMBER_FN(instance, SaveControlSettingsToINI)();
                  s_savingJoystickUsesNativeDefaults = false;
               }
            };
         };
         //
         void Inner(RE::OSInputGlobals* input) {
            XXNGamepadConfigManager::GetInstance().Init();
            RetroactiveINIFix::Execute(input);
            XXNGamepadConfigManager::GetInstance().ApplySelectedProfile(input);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  ecx, 0x00B07BF0; // ecx = g_IniSettingCollection
               call eax; // reproduce patched-over virtual call
               push ebx;
               call Inner;
               add  esp, 4;
               mov  eax, 0x0040467C;
               jmp  eax;
            }
         };
         void Apply() {
            WriteRelJump(0x00404675, (UInt32)&Outer); // OSInputGlobals::LoadControlBindingsFromINI
            SafeWrite16 (0x0040467A, 0x9090); // courtesy NOPs
         };
      };
      namespace ResetControls {
         //
         // Make ResetControls use values that are suitable for our input implementation.
         //
         __declspec(naked) void Outer() {
            _asm {
               mov dword ptr [ecx + 0x1BB8], 0xFFFFFFFF; // Move (WASD)  | Not Bound
               mov byte ptr [ecx + 0x1BBC], 0x11; // Use Equipped        | Right Trigger
               mov byte ptr [ecx + 0x1BBD], 0x0C; // Activate            | A
               mov byte ptr [ecx + 0x1BBE], 0x10; // Block               | Left Trigger
               mov byte ptr [ecx + 0x1BBF], 0x09; // Cast                | Right Bumper
               mov byte ptr [ecx + 0x1BC0], 0x0E; // Draw/Sheathe Weapon | X
               mov byte ptr [ecx + 0x1BC1], 0x06; // Sneak               | Left Stick Click
               mov byte ptr [ecx + 0x1BC2], 0xFF; // Run                 | Not Bound
               mov byte ptr [ecx + 0x1BC3], 0xFF; // Toggle Auto-Run     | Not Bound
               mov byte ptr [ecx + 0x1BC4], 0xFF; // Toggle Auto-Move    | Not Bound
               mov byte ptr [ecx + 0x1BC5], 0x0F; // Jump                | Y
               mov byte ptr [ecx + 0x1BC6], 0x07; // Toggle POV          | Right Stick Click
               mov byte ptr [ecx + 0x1BC7], 0x0D; // Menu Mode           | B
               mov byte ptr [ecx + 0x1BC8], 0x05; // Rest                | Back
               mov byte ptr [ecx + 0x1BC9], 0x00; // Quick Menu          | D-Pad Up // doesn't work?
               mov byte ptr [ecx + 0x1BCA], 0xFF; // Quick Key 1         | Not Bound
               mov byte ptr [ecx + 0x1BCB], 0xFF; // Quick Key 2         | Not Bound
               mov byte ptr [ecx + 0x1BCC], 0xFF; // Quick Key 3         | Not Bound
               mov byte ptr [ecx + 0x1BCD], 0xFF; // Quick Key 4         | Not Bound
               mov byte ptr [ecx + 0x1BCE], 0xFF; // Quick Key 5         | Not Bound
               mov byte ptr [ecx + 0x1BCF], 0xFF; // Quick Key 6         | Not Bound
               mov byte ptr [ecx + 0x1BD0], 0xFF; // Quick Key 7         | Not Bound
               mov byte ptr [ecx + 0x1BD1], 0xFF; // Quick Key 8         | Not Bound
               mov byte ptr [ecx + 0x1BD2], 0xFF; // Quicksave           | Not Bound
               mov byte ptr [ecx + 0x1BD3], 0xFF; // Quickload           | Not Bound
               mov byte ptr [ecx + 0x1BD4], 0x08; // Grab (Z-Key)        | Left Bumper
               mov eax, 0x00403B3F;
               jmp eax;
            };
         };
         void Apply() {
            WriteRelJump(0x00403AAC, (UInt32)&Outer);
            SafeWrite32 (0x00403AB1, 0x90909090); // courtesy NOPs
         };
      };
      namespace SensitivityFix {
         //
         // Oblivion multiplies the joystick sensitivity into the joystick input, 
         // adds that to the mouse input, and then multiplies the lot of it by 
         // the mouse sensitivity. This means that joystick input is downscaled 
         // twice, rendering the look stick inoperable.
         //
         // The various get and multiply operations are scattered all over the 
         // movement code. Since it's only looking that's broken, it's easiest 
         // to let everything else just work with the broken input, and then 
         // re-retrieve the inputs and scale them properly at look-time.
         //
         inline float _normalizeToFrameRate(float v) {
            float ft = RE::g_timeInfo->frameTime;
            if (ft > 1.0F)
               return v;
            return v * ft;
         }
         static struct {
            float x = 0.0F;
            float y = 0.0F;
            float time = 0.0F;
         } s_look;
         void _updateSensitivity() {
            auto& config = XXNGamepadConfigManager::GetInstance();
            auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
            if (gamepad) {
               float ft = std::min<float>(1.0F, RE::g_timeInfo->frameTime);
               s_look.time += ft;
               //
               float sensX = cobb::degrees_to_radians(config.sensitivityX);
               float sensY = cobb::degrees_to_radians(config.sensitivityY);
               {  // look acceleration
                  float accelTime = NorthernUI::INI::XInput::fJoystickSensAccelTime.fCurrent;
                  if (s_look.time < accelTime) {
                     float accelOffset = cobb::degrees_to_radians(NorthernUI::INI::XInput::fJoystickSensAccelOffset.fCurrent); // how far below the look sensitivity we start at
                     accelOffset *= 1 - (s_look.time / accelTime);
                     sensX -= accelOffset;
                     sensY -= accelOffset;
                  }
               }
               sensX *= ft;
               sensY *= ft;
               //
               float gamepadX = gamepad->GetJoystickAxis(RE::INI::Controls::iJoystickLookLeftRight->i);
               float gamepadY = gamepad->GetJoystickAxis(RE::INI::Controls::iJoystickLookUpDown->i);
               s_look.x = gamepadX * sensX;
               s_look.y = gamepadY * sensY;
               if (cobb::distance(gamepadX, gamepadY) <= 0.0)
                  s_look.time = 0.0F;
            } else {
               s_look.x = 0.0F;
               s_look.y = 0.0F;
               s_look.time = 0.0F;
            }
         }
         //
         namespace Registrations { // fix initial checks on look stick input; also call _updateSensitivity
            //
            // The first part of the movement code grabs the joystick input, 
            // scales it by a float, and stuffs it into a signed int. However, 
            // the scaling factor needs to be extremely small; even a scaling 
            // factor of 0.02 is WAY too large and results in uncontrollably 
            // fast turning.
            //
            // As such, if we're scaling by factors on the order of 0.002, then 
            // it is impossible for any joystick input to produce a value that 
            // won't simply be truncated to zero when cast to an int. This is 
            // a problem because no movement is carried out if the target value 
            // is an int.
            //
            // The solution is to just add 1 or -1 to the ints if there is any 
            // joystick movement, since the ints themselves aren't used to 
            // produce the look changes anyway.
            //
            SInt32 __stdcall Inner(float value) {
               return ((0.0F < value) - (value < 0.0F));
            };
            __declspec(naked) void OuterX() {
               _asm {
                  call _updateSensitivity;
                  push ecx;
                  fstp [esp];
                  call Inner; // stdcall
                  mov  ecx, 0x006719A1;
                  jmp  ecx;
               }
            };
            __declspec(naked) void OuterY() {
               _asm {
                  push ecx;
                  fstp [esp];
                  call Inner; // stdcall
                  mov  ecx, 0x006719CC;
                  jmp  ecx;
               }
            };
            void Apply() {
               //
               // X:
               //
               SafeWrite16 (0x00671996, 0x9090);
               SafeWrite32 (0x00671998, 0x90909090); // NOP out multiplication by joystick X-sensitivity INI setting
               WriteRelJump(0x0067199C, (UInt32)&OuterX);
               //
               // Y:
               //
               SafeWrite16 (0x006719C1, 0x9090);
               SafeWrite32 (0x006719C3, 0x90909090); // NOP out multiplication by joystick X-sensitivity INI setting
               WriteRelJump(0x006719C7, (UInt32)&OuterY);
            };
         };
         namespace Vanity {
            static bool s_walkBlessed = false;
            //
            void _stdcall Inner(float timestep) {
               auto input   = RE::OSInputGlobals::GetInstance();
               auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
               //
               float x = CALL_MEMBER_FN(input, GetMouseAxisMovement)(1) / 57;
               float y = CALL_MEMBER_FN(input, GetMouseAxisMovement)(2) / 57;
               x += s_look.x;
               y += s_look.y;
               x *= RE::GMST::fVanityModeXMult->f;
               y *= RE::GMST::fVanityModeYMult->f;
               if (!s_walkBlessed) {
                  //
                  // The vanilla game multiplies the vanity mode sensitivity settings by the current 
                  // timestep. WalkBlessed doesn't (and they overwrite the vanity mode sensitivity 
                  // settings in-memory with a smaller value, to compensate). If we multiply by the 
                  // timestep, then the camera turn speed becomes unusably slow when WalkBlessed is 
                  // installed; if we don't multiply by the timestep, then it becomes unusably fast 
                  // without WalkBlessed.
                  //
                  x *= timestep;
                  y *= timestep;
               }
               *RE::fVanityControlX += x;
               *RE::fVanityControlY += y;
            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, dword ptr [ebp + 0x8];
                  push eax;
                  call Inner; // stdcall
                  mov  eax, 0x00671B99;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x00671B56, (UInt32)&Outer);
               //
               s_walkBlessed = g_obse->GetPluginLoaded("WalkBlessed");
            }
         }
         namespace XAxis {
            float Inner() {
               auto input = RE::OSInputGlobals::GetInstance();
               float x = (float)CALL_MEMBER_FN(input, GetMouseAxisMovement)(RE::kMouseAxis_X) * RE::INI::Controls::fMouseSensitivity->f;
               //
               /*//
               auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
               if (gamepad) {
                  auto& config = XXNGamepadConfigManager::GetInstance();
                  float g = (float)gamepad->GetJoystickAxis(RE::INI::Controls::iJoystickLookLeftRight->i) * _normalizeToFrameRate(cobb::degrees_to_radians(config.sensitivityX));
                  x += g;
               }
               //*/
               x += s_look.x;
               return x;
            }
            __declspec(naked) void Outer() {
               _asm {
                  call Inner;
                  test esi, esi; // reproduce comparison that can get bulldozed by Inner
                  mov  ecx, 0x00671C5E;
                  jmp  ecx;
               };
            };
            void Apply() {
               WriteRelJump(0x00671C54, (UInt32)&Outer);
               SafeWrite8  (0x00671C59, 0x90);       // courtesy NOP
               SafeWrite32 (0x00671C5A, 0x90909090); // courtesy NOPs
            }
         };
         namespace XAxisUnknown {
            float Inner() {
               auto input = RE::OSInputGlobals::GetInstance();
               float x = (float)CALL_MEMBER_FN(input, GetMouseAxisMovement)(RE::kMouseAxis_X) / 24.0F;
               //
               auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
               if (gamepad) {
                  auto& config = XXNGamepadConfigManager::GetInstance();
                  float gX = (float)gamepad->GetJoystickAxis(RE::INI::Controls::iJoystickLookLeftRight->i);
               }
               return x;
            };
            __declspec(naked) void Outer() {
               _asm {
                  call Inner;
                  mov  ecx, 0x00671D51;
                  jmp  ecx;
               };
            };
            void Apply() {
               WriteRelJump(0x00671D43, (UInt32)&Outer);
               SafeWrite8  (0x00671D48, 0x90);       // NOPs to indicate skipped instructions
               SafeWrite16 (0x00671D49, 0x9090);     // NOPs to indicate skipped instructions
               SafeWrite32 (0x00671D4B, 0x90909090); // NOPs to indicate skipped instructions
               SafeWrite16 (0x00671D4F, 0x9090);     // NOPs to indicate skipped instructions
            };
         };
         namespace YAxis {
            float Inner() {
               auto input = RE::OSInputGlobals::GetInstance();
               float y = (float)CALL_MEMBER_FN(input, GetMouseAxisMovement)(RE::kMouseAxis_Y) * RE::INI::Controls::fMouseSensitivity->f;
               //
               /*//
               auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
               if (gamepad) {
                  auto& config = XXNGamepadConfigManager::GetInstance();
                  float g = (float)gamepad->GetJoystickAxis(RE::INI::Controls::iJoystickLookUpDown->i) * _normalizeToFrameRate(cobb::degrees_to_radians(config.sensitivityY));
                  y += g;
               }
               //*/
               y += s_look.y;
               if (RE::INI::Controls::bInvertYValues->b)
                  y = -y;
               return y;
            };
            __declspec(naked) void Outer() {
               _asm {
                  call Inner;
                  push esi;      // reproduce patched-over instruction
                  mov  ecx, ebx; // reproduce patched-over instruction (actually, skipped)
                  mov  eax, 0x00671D90;
                  jmp  eax;
               };
            };
            void Apply() {
               WriteRelJump(0x00671D83, (UInt32)&Outer);
               SafeWrite32 (0x00671D88, 0x90909090); // NOPs to denote skipped instructions
               SafeWrite32 (0x00671D8C, 0x90909090); // NOPs to denote skipped instructions
            };
         };
         void Apply() {
            Registrations::Apply();
            Vanity::Apply();
            XAxis::Apply();
            YAxis::Apply();
            XAxisUnknown::Apply();
         };
      };
      namespace RunFix {
         //
         // Gamepad movement controls have you run if either the X- or Y-axis joystick 
         // movement is in the outer 2%. This means that diagonal movement doesn't count 
         // as running, because X and Y are only in the outer 70%, circles being the 
         // tricky sirens they are.
         //
         bool ShouldRun(SInt32 x, SInt32 y) {
            float distance = (x*x) + (y*y);
            distance = sqrt(distance);
            return (distance > XXNGamepadConfigManager::GetInstance().sensitivityRun);
         };
         __declspec(naked) void Outer() {
            _asm {
               push ecx;
               push edi;
               call ShouldRun;
               add  esp, 8;
               test al, al;
               jnz  lShouldRun;
            lShouldNotRun:
               mov  eax, 0x006721B9;
               jmp  eax;
            lShouldRun:
               mov  eax, 0x006721AE;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x006721A4, (UInt32)&Outer);
         };
      };
      namespace AlwaysRunFix {
         //
         // Vanilla Oblivion handles running like this:
         //
         //  - If the gamepad joystick is in the outer 2%, send the Run key.
         //
         //  - If Auto-Run is enabled and the Run key isn't being sent, send 
         //    the Run key and set a movement flag on the player.
         //
         //     - Otherwise, if the Run key is being sent, set that same flag 
         //       on the player.
         //
         // As a result of this, if Auto-Run is enabled and the gamepad joy-
         // stick is in the outer 2%, that movement flag doesn't get set... 
         // causing Auto-Run to actually *invert* running behavior for the 
         // player. (This also has the effect of making it so that holding 
         // Run while Auto-Run is enabled causes you to walk.)
         //
         // I'd rather "always run" behavior not ever affect joystick input, 
         // but that would require more substantial changes to the input code.
         //
         bool _AllowAutoRun() {
            return NorthernUI::INI::XInput::bToggleAlwaysRunWorks.bCurrent;
         };
         __declspec(naked) void OuterA() {
            //
            // Hook for when the joystick is in the outer 2% (i.e. the player 
            // should run). We want to make the game set a flag that it forgets 
            // to set here.
            //
            _asm {
               mov  eax, 0x00403380; // OSInputGlobals::SendControlPress
               call eax;             // reproduce patched-over call
               or   dword ptr [esp + 0x14], 0x200;
               call _AllowAutoRun;
               test al, al;
               jz   lSkip;
               mov  eax, 0x006721B9;
               jmp  eax;
            lSkip:
               push 1;   // reproduce skipped instruction
               push 0xA; // reproduce skipped instruction
               mov  eax, 0x006721F6;
               jmp  eax;
            };
         };
         __declspec(naked) void OuterB() {
            //
            // Hook for when auto-run is checking whether Run is pressed, in 
            // order to know whether to fire a Run press.
            //
            _asm {
               test edi, edi;
               jnz  lJoystickHandling;
               mov  ecx, dword ptr [esp + 0x24]; // esp1C, but two args were already pushed by vanilla code
               test ecx, ecx;
               jnz  lJoystickHandling;
            lStandardExit:
               mov  edi, dword ptr [esp + 0x2C]; // reproduce patched-over instruction
               mov  ecx, edi;                    // reproduce patched-over instruction
               mov  eax, 0x006721CC;
               jmp  eax;
            lJoystickHandling:
               call _AllowAutoRun;
               test al, al;
               jnz  lStandardExit;
               mov  eax, 0x006721F6;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x006721B4, (UInt32)&OuterA);
            WriteRelJump(0x006721C6, (UInt32)&OuterB);
            SafeWrite8  (0x006721CB, 0x90);
         };
      };
      namespace DontSaveOurMappings {
         //
         // Ensure our joystick mappings aren't written to Oblivion.ini. Just preserve the 
         // values already in the INI file.
         //
         UInt8 __stdcall _GetJoystickMappingFromINI(UInt8 index, const char* key, const char* filename) {
            if (s_savingJoystickUsesNativeDefaults) { // used for the retroactive fix; see LoadControls::RetroactiveINIFix
               return RE::g_defaultJoystickMappings[index];
            }
            char buf[128];
            GetPrivateProfileString("Controls", key, "", buf, sizeof(buf), filename); // this is slow, but Bethesda uses it anyway, so we'll use it too
            char* end = nullptr;
            UInt32 code = strtol(buf, &end, 16); // 0x00XXYYZZ given X = keyboard, Y = mouse, Z = joystick
            return code & 0xFF;
         };
         __declspec(naked) void Outer() {
            _asm {
               lea eax, [esp + 0x50]; // filename
               mov ecx, dword ptr [edi];
               push eax;
               push ecx;
               mov  eax, 0x1D;
               sub  al, bl;
               push eax;
               call _GetJoystickMappingFromINI; // stdcall; don't need to clean up stack
               movzx ecx, eax;
               xor eax, eax; // patched-over instruction
               mov edx, 0x004044D7;
               jmp edx;
            };
         };
         void Apply() {
            WriteRelJump(0x004044D1, (UInt32)&Outer); // OSInputGlobals::SaveControlSettingsToINI
            SafeWrite8  (0x004044D6, 0x90); // courtesy NOP
         };
      };
      namespace UICursorGamepadControl {
         //
         // Allow the gamepad to control the cursor: LS to move; RS vertical for scroll wheel. 
         // Per checks in UISupport, when this is enabled, LS doesn't fire keyboard navigation.
         //
         // We need this for LockPickMenu gamepad support (though that menu also needs some 
         // additional patches; it checks cursor position AND mouse input separately). At 
         // present, we just have LockPickMenu toggle this on and off, and it's the only 
         // thing that toggles it on and off.
         //
         constexpr float joystickCursorXSpeed = 1; // floats for speed, but in practice only integers will work
         constexpr float joystickCursorYSpeed = 1;
         constexpr float joystickCursorZSpeed = 1;
         //
         SInt32 Inner(RE::OSInputGlobals* input, UInt32 mouseAxis) {
            SInt32 movement = CALL_MEMBER_FN(input, GetMouseAxisMovement)(mouseAxis);
            if (g_uiCursorGamepadControlEnabled) {
               auto gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
               if (gamepad) {
                  auto& config = XXNGamepadConfigManager::GetInstance();
                  UInt32 axis;
                  float  mult;
                  switch (mouseAxis) {
                     case RE::kMouseAxis_X:
                        axis = config.swapSticksMenuMode ? XXNGamepad::kJoystickAxis_RightX : XXNGamepad::kJoystickAxis_LeftX;
                        mult = joystickCursorXSpeed;
                        break;
                     case RE::kMouseAxis_Y:
                        axis = config.swapSticksMenuMode ? XXNGamepad::kJoystickAxis_RightY : XXNGamepad::kJoystickAxis_LeftY;
                        mult = joystickCursorYSpeed;
                        break;
                     case RE::kMouseAxis_Scroll:
                        axis = config.swapSticksMenuMode ? XXNGamepad::kJoystickAxis_LeftY : XXNGamepad::kJoystickAxis_RightY;
                        mult = joystickCursorZSpeed;
                        break;
                  }
                  float mod = (float)gamepad->GetJoystickAxis(axis) / 100.0F * mult;
                  movement += mod;
               }
            }
            return movement;
         };
         __declspec(naked) void Outer(UInt32 mouseAxis) {
            _asm {
               mov  eax, dword ptr [esp + 0x4];
               push eax;
               push ecx;
               call Inner;
               add  esp, 8;
               retn 4;
            };
         };
         //
         void Apply() {
            //
            // Patch InterfaceManager::UpdateCursorState:
            //
            WriteRelCall(0x0057E80E, (UInt32)&Outer); // X-axis
            WriteRelCall(0x0057E81B, (UInt32)&Outer); // Y-axis
            //
            // Patch InterfaceManager::Update:
            //
            WriteRelCall(0x005823E6, (UInt32)&Outer); // scroll wheel
            //
            // Patch LockPickMenu::Subroutine005AFA80:
            //
            // NEVER MIND: LockPickMenu PATCHES THESE ITSELF
            //
            //WriteRelCall(0x005AFA92, (UInt32)&Outer); // X-axis
            //WriteRelCall(0x005AFA9B, (UInt32)&Outer); // Y-axis
         };
      };
      namespace UISupport {
         namespace JournalHandler {
            __declspec(naked) void LimitCheckToKeyboard(UInt32 control, UInt32 state) {
               _asm {
                  // this == OSInputGlobals*
                  mov   eax, dword ptr[esp + 0x4];
                  mov   al, byte ptr[eax + ecx + 0x1B7E]; // eax = this->keyboard[control]
                  movzx eax, al;
                  mov   edx, dword ptr[esp + 0x8];
                  push  edx;
                  push  eax;
                  push  0; // force to keyboard scheme
                  mov   eax, 0x00403490; // OSInputGlobals::QueryControlState
                  call  eax;
                  retn  8;
               };
            };
            void Apply() {
               //
               // Limit the vanilla journal input checks to keyboard-only. We'll reproduce 
               // the journal functionality for the gamepad.
               //
               //WriteRelCall(0x00583A9B, (UInt32)&LimitCheckToKeyboard); // close
               WriteRelCall(0x00583B20, (UInt32)&LimitCheckToKeyboard); // open
            };

            void DoJournalOpen() {
               XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
               if (!gamepad)
                  return;
               auto ui    = RE::InterfaceManager::GetInstance();
               auto input = RE::OSInputGlobals::GetInstance();
               bool press = gamepad->GetButtonState(input->joystick[RE::kControl_MenuMode], RE::kKeyQuery_Down);
               SInt32 already = CALL_MEMBER_FN(ui->menuRoot, GetFloatTraitValue)(0x1771);
               UInt32 shortcutTo = 0; // vanilla code would set this to a Big Four menu ID if F1 - F4 are pressed
               if ((press || shortcutTo) // at 0x00583AA6
               && (
                  (*(RE::PlayerCharacter**)g_thePlayer)->unk5C0 == false
                  && !(*g_thePlayer)->IsDead(0)
                  && CALL_MEMBER_FN(ui, Subroutine0057D240)(0)
                  && ui->unk008 == 1
                  && ui->activeMenuIDs[0] == 0
                  )
               ) {
                  // at 0x00583AE7
                  CALL_MEMBER_FN(ui->menuRoot, UpdateFloat)(0x1771, shortcutTo); // Big Four menu ID to show?
                  ThisStdCall(0x00663920, *g_thePlayer); // (*g_thePlayer)->TES4_00663920();
                  CALL_MEMBER_FN(ui, AddMenuToActiveIDStack)(RE::InterfaceManager::kMenuMode_BigFour);
                  RE::ShowOrRefreshBigFour();
               }
            };
            bool DoJournalClose() { // returns true if the menu is closed
               XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
               if (!gamepad)
                  return false;
               auto ui    = RE::InterfaceManager::GetInstance();
               auto input = RE::OSInputGlobals::GetInstance();
               bool press = gamepad->GetButtonState(XXNGamepad::kGamepadButton_B, RE::kKeyQuery_Down);
               SInt32 already = CALL_MEMBER_FN(ui->menuRoot, GetFloatTraitValue)(0x1771);
               UInt32 shortcutTo = 0; // vanilla code would set this to a Big Four menu ID if F1 - F4 are pressed
               if ( // at 0x00583B15
                  (press || (shortcutTo && shortcutTo == already))
               && (
                  (*(RE::PlayerCharacter**)g_thePlayer)->unk5C0 == false
                  && CALL_MEMBER_FN(ui, Subroutine0057D240)(0)
                  && ui->unk008 == 2
                  && CALL_MEMBER_FN(ui, Subroutine0057CFE0)(RE::InterfaceManager::kMenuMode_BigFour, 0) >= 0 // pretty sure this is responsible for most of the work done to close the menu
                  )
               ) {
                  auto sound = RE::OSSoundGlobals::GetInstance();
                  if (sound) {
                     auto edi = CALL_MEMBER_FN(sound, Subroutine006ADE50)("UIInventoryClose", 0x121, 1); // this sound is silent/unused, so no, it doesn't prove that this is "close" code
                     if (edi) {
                        CALL_MEMBER_FN(edi, Subroutine006B7190)(0);
                        CALL_MEMBER_FN(edi, Destructor)();
                        FormHeap_Free(edi);
                     }
                  }
                  RE::HideBigFour(); // pretty sure this just does some non-essential cleanup // at 0x00583BA5
                  return true;
               }
               return false;
            };
         };
         namespace ClearButtonStateOnMenuOpen {
            RE::InterfaceManager* Inner() {
               XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
               if (gamepad)
                  gamepad->IgnoreButtons();
               //
               return RE::InterfaceManager::GetInstance(); // reproduce patched-over call
            }
            void Apply() {
               WriteRelCall(0x00585198, (UInt32)&Inner);
            };
         };

         inline void _CheckUIKey(RE::InterfaceManager* ui, XXNGamepad* gamepad, XXNGamepad::Button key, UInt32 send) {
            if (gamepad->GetButtonState(key, XXNGamepad::KeyQuery::kKeyQuery_Down))
               CALL_MEMBER_FN(ui, HandleNavigationKeypress)(send);
         };

         void Inner(RE::InterfaceManager* ui) {
            XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
            if (!gamepad)
               return;
            auto& config = XXNGamepadConfigManager::GetInstance();
            //
            // We check key-down, specifically, because if we check key-up, then keys that were pressed before 
            // a menu opened can trigger keyboard navigation in that menu when released. In the worst cases, a 
            // menu that was opened by a gamepad keypress can end up responding to that same keypress.
            //
            #if XXN_USE_UI_SIDE_PAUSE == 1
               //
               // The vanilla game checks for the Start button KeyUp in order to toggle the start menu. However,
               // HandleNavigationKeypress also pops the start menu (and, like, why?!). This means that if we
               // try to listen for Start KeyDown, then we'll end up toggling the start menu twice every time
               // the Start button is pressed in menu mode (so pressing Start with any other menu open would
               // just open and instantly close the start menu, and trying to close the start menu with Start
               // would just close and instantly reopen it).
               //
               _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_Start, RE::InterfaceManager::kNavigationKeypress_XboxStart);
            #endif
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_A, RE::InterfaceManager::kNavigationKeypress_XboxA);
            {  // We reroute the B button for closing the Big Four.
               if (ui->activeMenuIDs[0] == RE::InterfaceManager::kMenuMode_BigFour && ui->activeMenuIDs[1] == 0) {
                  if (JournalHandler::DoJournalClose())
                     return;
               } else {
                  _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_B, RE::InterfaceManager::kNavigationKeypress_XboxB);
               }
            }
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_X, RE::InterfaceManager::kNavigationKeypress_XboxX);
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_Y, RE::InterfaceManager::kNavigationKeypress_XboxY);
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_LB, RE::InterfaceManager::kNavigationKeypress_XboxLB);
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_RB, RE::InterfaceManager::kNavigationKeypress_XboxRB);
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_SpecialCaseLT, RE::InterfaceManager::kNavigationKeypress_XboxLT);
            _CheckUIKey(ui, gamepad, XXNGamepad::kGamepadButton_SpecialCaseRT, RE::InterfaceManager::kNavigationKeypress_XboxRT);
            {  // directions
               static UInt32 joystickLastFire  = 0;
               static UInt32 joystickRate      = NorthernUI::INI::XInput::uMenuJoystickRateInitial.iCurrent;
               static SInt32 joystickDirection = -1;
               static UInt32 joystickRateCount = 0;
               static UInt32 dPadLastFire      = 0;
               static bool   dPadRepeating     = false;
               UInt32 current = RE::g_timeInfo->unk10;
               bool   didDPad = false;
               if (NorthernUI::INI::XInput::bMenuConsumesDPad.bCurrent) { // D-Pad
                  SInt8 x = 0;
                  SInt8 y = 0;
                  x = gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadRight, XXNGamepad::KeyQuery::kKeyQuery_Hold) - gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadLeft, XXNGamepad::KeyQuery::kKeyQuery_Hold);
                  y = gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadDown,  XXNGamepad::KeyQuery::kKeyQuery_Hold) - gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadUp,   XXNGamepad::KeyQuery::kKeyQuery_Hold);
                  if (x || y) {
                     bool fire = false;
                     //
                     // We want the D-Pad to behave consistently with the arrow keys: once you've 
                     // held any D-Pad button down for fKeyRepeatTime seconds, we will fire input 
                     // events every fKeyRepeatInterval seconds.
                     //
                     if (dPadRepeating) {
                        fire = (current - dPadLastFire) >= RE::INI::Interface::fKeyRepeatInterval->f;
                     } else {
                        if (!dPadLastFire) {
                           fire = true;
                        } else {
                           fire = dPadRepeating = (current - dPadLastFire >= RE::INI::Interface::fKeyRepeatTime->f);
                        }
                     }
                     if (fire) {
                        dPadLastFire = current;
                        didDPad = true;
                        if (x)
                           CALL_MEMBER_FN(ui, HandleNavigationKeypress)(x > 0 ? RE::InterfaceManager::kNavigationKeypress_Right : RE::InterfaceManager::kNavigationKeypress_Left);
                        if (y)
                           CALL_MEMBER_FN(ui, HandleNavigationKeypress)(y > 0 ? RE::InterfaceManager::kNavigationKeypress_Down : RE::InterfaceManager::kNavigationKeypress_Up);
                     }
                  } else {
                     dPadLastFire  = 0;
                     dPadRepeating = false;
                  }
               }
               if (!didDPad && !CobbPatches::XboxGamepad::g_uiCursorGamepadControlEnabled) {
                  //
                  // We don't want to respond to non-zero joystick input every frame, because every response 
                  // is equivalent to an arrow keypress. If the user holds the joystick down for five frames, 
                  // we don't want them moving down five items in a list! We need to use a static variable 
                  // and gate out our response on a time interval (resetting our timing when the joystick is 
                  // zeroed).
                  //
                  SInt32 x =  gamepad->GetJoystickAxis(config.swapSticksMenuMode ? XXNGamepad::kJoystickAxis_RightX : XXNGamepad::kJoystickAxis_LeftX);
                  SInt32 y = -gamepad->GetJoystickAxis(config.swapSticksMenuMode ? XXNGamepad::kJoystickAxis_RightY : XXNGamepad::kJoystickAxis_LeftY); // TODO: I... I think we just... shouldn't negate it in the getter. Only the game's movement code and the MapMenu support need the Y-axis flipped, so why not just patch those getters to do it themselves.
                  if (x || y) {
                     {  // Determine current direction.
                        SInt32 currentDir;
                        //
                        double angle = atan2(y, x) * 180.0 / 3.141592653589793;
                        if (angle < 0.0F)
                           angle += 360.0F;
                        if (angle < 45.0F || angle > 315.0F) {
                           currentDir = RE::InterfaceManager::kNavigationKeypress_Right;
                        } else if (angle > 45.0F && angle < 135.0F) {
                           currentDir = RE::InterfaceManager::kNavigationKeypress_Up;
                        } else if (angle > 135.0F && angle < 225.0F) {
                           currentDir = RE::InterfaceManager::kNavigationKeypress_Left;
                        } else if (angle > 225.0F && angle < 315.0F) {
                           currentDir = RE::InterfaceManager::kNavigationKeypress_Down;
                        }
                        if (joystickDirection != currentDir) {
                           joystickLastFire  = 0;
                           joystickRate      = NorthernUI::INI::XInput::uMenuJoystickRateInitial.iCurrent;
                           joystickRateCount = 0;
                        }
                        joystickDirection = currentDir;
                     }
                     //
                     // We want the joystick to "accelerate" the longer it's held in the same direction. 
                     // As such, we'll fire events every (joystickRate) milliseconds. Every time we fire 
                     // an event, we increment (joystickRateCount). When it exceeds (joystickRateUsedFor), 
                     // we decrease (joystickRate) by (joystickRateMod) constraining it to a minimum of 
                     // (joystickRateMin), and we reset the (joystickRateCount) counter.
                     //
                     bool fire = false;
                     if (joystickLastFire) {
                        if (current - joystickLastFire >= joystickRate) {
                           fire = true;
                           if (++joystickRateCount >= NorthernUI::INI::XInput::uMenuJoystickRateUsedFor.iCurrent) {
                              joystickRate = (std::max)((UInt32)NorthernUI::INI::XInput::uMenuJoystickRateMin.iCurrent, joystickRate - NorthernUI::INI::XInput::uMenuJoystickRateMod.iCurrent);
                              joystickRateCount = 0;
                           }
                        }
                     } else {
                        fire = true;
                     }
                     if (fire) {
                        joystickLastFire = current;
                        CALL_MEMBER_FN(ui, HandleNavigationKeypress)(joystickDirection);
                     }
                  } else { // joystick isn't being moved
                     joystickLastFire  = 0;
                     joystickRate      = NorthernUI::INI::XInput::uMenuJoystickRateInitial.iCurrent;
                     joystickRateCount = 0;
                     joystickDirection = -1;
                  }
               } else { // d-pad was pressed
                  joystickLastFire  = 0;
                  joystickRate      = NorthernUI::INI::XInput::uMenuJoystickRateInitial.iCurrent;
                  joystickRateCount = 0;
                  joystickDirection = -1;
               }
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x00403520;
               call eax;
               push eax; // preserve return value
               mov  ecx, esi;
               push ecx;
               call Inner;
               add  esp, 4;
               pop  eax; // restore return value
               mov  ecx, 0x0058369B;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x00583696, (UInt32)&Outer);
            JournalHandler::Apply();
            // TODO: Have the Inner hook call Journal::Recreate() when appropriate!
            ClearButtonStateOnMenuOpen::Apply(); // TEST
         };
      };
      namespace UICursorAutoHide {
         enum State {
            kCursorState_Hidden = 0,
            kCursorState_Hiding = 1,
            kCursorState_Shown = 2,
         };

         volatile static SInt32 s_rightfulAlpha = 255; // alpha the cursor would have if it weren't auto-hidden
         volatile static bool   s_showCursor = true;
         volatile static State  s_cursorCurrentState = kCursorState_Shown;
         volatile static UInt32 s_lastShowedAt = 0;

         namespace CursorAlpha {
            void PatchedEnd(SInt32 alpha) {
               s_rightfulAlpha = alpha;
               if (alpha) {
                  s_showCursor = true;
                  s_cursorCurrentState = kCursorState_Shown;
                  s_lastShowedAt = RE::g_timeInfo->unk10;
               }
            };
            void Apply() {
               WriteRelJump(0x00583E2B, (UInt32)&PatchedEnd); // SetInterfaceManagerCursorAlpha+0x3B
            };
         };
         namespace CursorUpdateHook {
            static bool s_cursorMoved       = false;
            static bool s_mouseActive       = false; // pressing a button, etc.
            static bool s_mustHideNextFrame = false;

            namespace Start {
               void Inner() {
                  s_cursorMoved = !XXNGamepadSupportCore::GetInstance()->anyConnected;
                  s_mouseActive = CALL_MEMBER_FN(RE::OSInputGlobals::GetInstance(), QueryMouseKeyState)(RE::kMouseControl_Left, RE::kKeyQuery_Hold);
               };
               __declspec(naked) void Outer() {
                  _asm {
                     mov  eax, 0x0057E390; // GetNormalizedScreenMinY;
                     call eax;             // reproduce patched-over call
                     call Inner;
                     mov  ecx, 0x0057E911;
                     jmp  ecx;
                  };
               };
               void Apply() {
                  WriteRelJump(0x0057E90C, (UInt32)&Outer); // InterfaceManager::UpdateCursorState+0x14C
               };
            };
            namespace IfChanged {
               __declspec(naked) void Outer() {
                  _asm {
                     mov  s_cursorMoved, 1;
                     mov  eax, 0x0057D7A0; // GetNormalizedScreenWidth
                     call eax;             // reproduce patched-over call
                     mov  eax, 0x0057E9A9;
                     jmp  eax;
                  };
               };
               void Apply() {
                  WriteRelJump(0x0057E9A4, (UInt32)&Outer);
               };
            };
            namespace End {
               void __stdcall Inner(RE::InterfaceManager* ui) {
                  if (s_mustHideNextFrame && !s_mouseActive) {
                     s_mustHideNextFrame = false;
                     //
                     auto node = ui->cursor->renderedNode;
                     if (node->m_children.firstFreeEntry) {
                        auto child = (RE::NiAVObject*) node->m_children.data[0];
                        if (child) {
                           auto prop = (RE::NiMaterialProperty*) CALL_MEMBER_FN(child, GetPropertyByType)(RE::NiMaterialProperty::kType);
                           if (prop)
                              prop->alpha = 0.0F;
                        }
                     }
                     s_showCursor = false;
                     s_cursorCurrentState = kCursorState_Hidden;
                     s_lastShowedAt = 0;
                     return;
                  }
                  auto now = RE::g_timeInfo->unk10;
                  if (s_mouseActive || s_cursorMoved) {
                     s_showCursor   = true;
                     s_lastShowedAt = now;
                  } else if (s_showCursor) {
                     if (s_lastShowedAt) {
                        if (now - s_lastShowedAt >= NorthernUI::INI::Display::fAutoHideCursorDelay.fCurrent * /*to ms:*/1000)
                           s_showCursor = false;
                     } else
                        s_lastShowedAt = now;
                  }
                  if (!s_showCursor && s_cursorCurrentState != kCursorState_Hidden) {
                     if (ui->activeTile && ui->activeMenu) {
                        //
                        // If the cursor is hidden, treat that as mouseout. We want this for things 
                        // like MapMenu where you can move the cursor over a map icon, let it hide, 
                        // and then (if we don't clear the mouseover state) press A to activate the 
                        // icon under the hidden cursor -- not intuitive! The alternative for that 
                        // case is to have the code here query MapMenu to try and find the hovered 
                        // map icon, which is... not great.
                        //
                        auto tile = ui->activeTile;
                        auto menu = ui->activeMenu;
                        if (menu->unk24 == 1) {
                           CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_mouseover, 0.0);
                           SInt32 id = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_id);
                           menu->HandleMouseout(id, tile);
                           ui->activeTile = nullptr;
                           ui->activeMenu = nullptr;
                        }
                     }
                  }
                  if (s_showCursor && s_cursorCurrentState == kCursorState_Shown)
                     return;
                  //
                  // DO NOT skip the code below if the cursor is already fully hidden. Something else -- 
                  // maybe vanilla, maybe another DLL -- is forcing the cursor alpha to visible when 
                  // menus open, and I can't find what. This is WAY too minor to be worth the time I've 
                  // spent on it already, so just run the below on every frame we're supposed to be 
                  // hiding the cursor on.
                  //
                  s_cursorCurrentState = s_showCursor ? kCursorState_Shown : kCursorState_Hiding;
                  //
                  auto node = ui->cursor->renderedNode;
                  if (node->m_children.firstFreeEntry == 0)
                     return;
                  auto child = (RE::NiAVObject*) node->m_children.data[0];
                  if (!child)
                     return;
                  auto prop = (RE::NiMaterialProperty*) CALL_MEMBER_FN(child, GetPropertyByType)(RE::NiMaterialProperty::kType);
                  if (!prop)
                     return;
                  switch (s_cursorCurrentState) {
                     case kCursorState_Shown:
                        prop->alpha    = s_rightfulAlpha; // NOTE: Alpha should be [0, 1], but Bethesda just jams 255 into there to show the cursor...
                        s_lastShowedAt = now;
                        break;
                     case kCursorState_Hiding:
                        prop->alpha = (std::min)(1.0F, prop->alpha) - (2.0F * RE::g_timeInfo->frameTime); // fade out over half a second (one second, twice as fast)
                        if (prop->alpha <= 0.0F) {
                           prop->alpha = 0.0F;
                           s_cursorCurrentState = kCursorState_Hidden;
                        }
                        break;
                     case kCursorState_Hidden: // brute-force the cursor to hidden if something else is messing with it
                        if (prop->alpha) {
                           prop->alpha = 0.0F;
                           return;
                        }
                  }
                  prop->count++;
               };
               __declspec(naked) void Outer() {
                  _asm {
                     push esi;
                     call Inner; // stdcall
                     fldz;     // reproduce patched-over instruction
                     push 1;   // reproduce patched-over instruction
                     push ecx; // reproduce patched-over instruction
                     mov  eax, 0x0057EA0B;
                     jmp  eax;
                  };
               };
               void Apply() {
                  WriteRelJump(0x0057EA06, (UInt32)&Outer);
               };
            };
            namespace ForceHideOnAllMenusClosed {
               //
               // Consider this situation: you move the mouse when a menu is open, close 
               // that menu before the cursor fades, and return to gameplay. Then, ages 
               // later, you open another menu. The cursor will still be visible. Yet if 
               // instead, you move the mouse when a menu is open, let the cursor fade, 
               // and then close that menu, play the game, and open another menu, then the 
               // cursor will be invisible.
               //
               // This hook lets us set the cursor fade state whenever all active menus 
               // (i.e. all menus that can take input) are closed.
               //
               void _stdcall Inner() {
                  auto ui = *RE::ptrInterfaceManager;
                  {  // Verify that no active menus remain
                     UInt8 i = 0;
                     do {
                        auto id = ui->activeMenuIDs[i];
                        if (id) {
                           return;
                        }
                     } while (++i < std::extent<decltype(RE::InterfaceManager::activeMenuIDs)>::value);
                  }
                  s_mustHideNextFrame = true;
               }
               __declspec(naked) void Outer() {
                  _asm {
                     call Inner; // stdcall
                     mov  eax, 0x00582160; // reproduce patched-over call to InterfaceManager_GetSingleton
                     call eax;             //
                     mov  ecx, 0x005847F4;
                     jmp  ecx;
                  }
               }
               void Apply() {
                  WriteRelJump(0x005847EF, (UInt32)&Outer); // Menu::FadeOut+0xAF
               }
            }
            namespace DontReShowCursorAfterLoadingScreen {
               //
               // ShowLoadingMenu sets the cursor alpha to zero so that you don't see the 
               // cursor on loading screens. LoadingMenu::~LoadingMenu sets the alpha back 
               // to full. That messes with us per the above.
               //
               void Inner(UInt8 desiredAlpha) {
                  //
                  // LoadingMenu hid the cursor. If we're only just now coming out of the 
                  // loading screen, and if we're not reverting the alpha, then the cursor 
                  // is still hidden; update our state vars accordingly:
                  //
                  s_showCursor = false;
                  s_cursorCurrentState = kCursorState_Hidden;
               }
               void Apply() {
                  WriteRelCall(0x005ADC11, (UInt32)&Inner); // replace call to SetInterfaceManagerCursorAlpha in LoadingMenu::~LoadingMenu
               }
            }
            namespace NoMouseoverIfCursorHidden {
               //
               // Setting the cursor's alpha to zero doesn't prevent it from mouseovering things, 
               // which can cause problems, most notably in MapMenu.
               //
               bool _stdcall Inner() {
                  if (s_cursorCurrentState == kCursorState_Hidden)
                     return false;
                  return true;
               }
               __declspec(naked) void Outer() {
                  _asm {
                     je   lNoMouse;
                     call Inner;
                     test al, al;
                     jz   lNoMouse;
                     mov  eax, 0x00582403;
                     jmp  eax;
                  lNoMouse:
                     mov  eax, 0x005833FC;
                     jmp  eax;
                  }
               }
               void Apply() {
                  WriteRelJump(0x005823FD, (UInt32)&Outer);
                  SafeWrite8(0x005823FD + 5, 0x90); // courtesy NOP
               }
            }
            
            void Apply() {
               Start::Apply();
               IfChanged::Apply();
               End::Apply();
               ForceHideOnAllMenusClosed::Apply();
               DontReShowCursorAfterLoadingScreen::Apply();
               NoMouseoverIfCursorHidden::Apply();
            };
         };
         void Apply() {
            //CursorAlpha::Apply();
            CursorUpdateHook::Apply();
         };
      };

      __declspec(naked) SInt32 GetJoystickAxisMovement(UInt32 gamepad, UInt32 axis) {
         _asm {
            call XXNGamepadSupportCore::GetInstance;
            mov  ecx, eax;
            mov  eax, dword ptr [esp + 4];
            push eax;
            call XXNGamepadSupportCore::GetGamepad;
            test eax, eax;
            je   lReturnZero;
            mov  edx, dword ptr [esp + 8];
            push edx;
            mov  ecx, eax;
            call XXNGamepad::GetJoystickAxis;
            retn 8;
         lReturnZero:
            mov  eax, 0;
            retn 8; // we are replacing a member function
         };
      };
      __declspec(naked) bool QueryJoystickButtonState(UInt8 gamepad, UInt8 button, UInt8 state) {
         _asm {
            call XXNGamepadSupportCore::GetInstance;
            mov  ecx, eax;
            mov  eax, dword ptr [esp + 4];
            push eax;
            call XXNGamepadSupportCore::GetGamepad;
            test eax, eax;
            je   lReturnFalse;
            mov  ecx, eax;
            mov  edx, dword ptr [esp + 8];
            mov  eax, dword ptr [esp + 0xC];
            push eax;
            push edx;
            call XXNGamepad::GetButtonState;
            retn 0xC;
         lReturnFalse:
            xor  eax, eax;
            retn 0xC;
         };
      };
      __declspec(naked) void Hook_SendControlPress() {
         _asm { // given esi instanceof InputManager && ecx == InputManager::Control
            mov  dl, byte ptr [esi + 0x1BB8 + ecx];   // UInt8 dl = esi->joystick[ecx];
            movzx edi, dl;                            // edi = (UInt32) dl;
            xor  edx, edx;                            // UInt32 i = 0;
         lLoopStart:                                  // do {
            call XXNGamepadSupportCore::GetInstance;  //
            mov  ecx, eax;                            //
            push edx;                                 //
            call XXNGamepadSupportCore::GetGamepad;   //    eax = XXNGamepadSupportCore::GetInstance()->GetGamepad(i);
            inc  edx;                                 //    i++;
            test eax, eax;                            //    if (!eax)
            je   lLoopStart;                          //       continue;
            mov  cl, byte ptr [eax + 1];              //    ecx = eax->isConnected;
            test cl, cl;                              //    if (!(byte)ecx)
            je   lLoopStart;                          //       continue;
            push edi;                                 //
            mov  ecx, eax;                            //
            call XXNGamepad::SendControlPress;        //    eax->SendControlPress(edi);
            cmp  edx, 4;                              //
            jl   lLoopStart;                          // } while (i < 4);
            pop  edi;                                 //
            pop  esi;                                 //
            retn 4;                                   // return;
         };
      };
      void Hook_UpdateGamepads() {
         auto core = XXNGamepadSupportCore::GetInstance();
         core->Update();
      };

      void OnINIChange(NorthernUI::INI::INISetting* s, NorthernUI::INI::ChangeCallbackArg oldVal, NorthernUI::INI::ChangeCallbackArg newVal) {
         auto core = XXNGamepadSupportCore::GetInstance();
         {  // bDontUseEvenWhenPatched
            auto& desired = NorthernUI::INI::XInput::bDontUseEvenWhenPatched;
            if (!s || s == &desired) {
               if (desired.bCurrent)
                  core->Disable();
               else
                  core->Enable();
            }
         }
      };
      void Apply() {
         if (!NorthernUI::INI::XInput::bEnabled.bCurrent) {
            PatchManager::GetInstance().FireEvent(PatchManager::Req::P_XInput);
            return;
         }
         g_xInputDllHandle = LoadLibrary("XInput9_1_0.dll");
         if (g_xInputDllHandle) {
            _MESSAGE("[Patch] XboxGamepad: Loaded XInput.");
            g_xInputPatchApplied = true;
            //
            // Patches:
            //
            {  // OSInputGlobals::PollAndUpdateInputState
               WriteRelCall(0x004046B8, (UInt32) &Hook_UpdateGamepads);
               SafeMemset  (0x004046BD, 0x90, 0x0040472F - 0x004046BD);
            }
            WriteRelJump(0x00402F50, (UInt32) &GetJoystickAxisMovement); // needs NOPs // Replace vanilla OSInputGlobals::GetJoystickAxisMovement
            {
               WriteRelJump(0x00402FC0, (UInt32) &QueryJoystickButtonState);
               //
               // Patch OSInputGlobals::QueryInputState to compare the specified joystick number to 4 
               // instead of to OSInputGlobals::joystickCount:
               //
               SafeWrite8(0x004034AA, 0x83);
               SafeWrite8(0x004034AB, 0xF8);
               SafeWrite8(0x004034AC, 0x04);
               SafeWrite8(0x004034AD, 0x90);
               SafeWrite8(0x004034AE, 0x90);
               SafeWrite8(0x004034AF, 0x90);
               //
               // Patch OSInputGlobals::QueryInputState to never check the POV controls; all keycodes 
               // should be fed to our button hook.
               //
               SafeWrite16(0x004034BA, 0x9090);
            }
            WriteRelJump(0x004033E8, (UInt32) &Hook_SendControlPress);
            PauseKey::Apply();
            DontSaveOurMappings::Apply();
            LoadControls::Apply();
            ResetControls::Apply();
            UICursorGamepadControl::Apply();
            UISupport::Apply();
            FixMovementZeroing::Apply();
            SensitivityFix::Apply();
            AlwaysRunFix::Apply();
            RunFix::Apply();
            UICursorAutoHide::Apply();
            //
            _MESSAGE("[Patch] XboxGamepad: Subroutines patched.");
            //
            OnINIChange(nullptr, 0, NorthernUI::INI::XInput::bDontUseEvenWhenPatched.bCurrent);
            NorthernUI::INI::RegisterForChanges(&OnINIChange);
         } else {
            _MESSAGE("[Patch] XboxGamepad: Failed to load XInput.");
         }
         PatchManager::GetInstance().FireEvent(PatchManager::Req::P_XInput);
      };
   };
};