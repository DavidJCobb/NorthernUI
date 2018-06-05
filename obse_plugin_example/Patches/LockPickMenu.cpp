#include "LockPickMenu.h"
#include "obse_common/SafeWrite.h"

#include "XboxGamepad/Main.h"
#include "XboxGamepad/Patch.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/LockPickMenu.h"

namespace CobbPatches {
   namespace LockPickMenu {
      //
      // A few issues with LockPickMenu:
      //
      //  - The X and A button mappings are hardcoded into HandleNavigationInput. They 
      //    always map to Auto-Attempt and to a minigame click.
      //
      //  - The A button is mapped incorrectly! It forwards to HandleMouseUp, but there 
      //    IS no mouse-up handler. All click logic is handled in HandleMouseDown.
      //
      //  - Because there is no HandleMouseUp, clicks forwarded from keyboard navigation 
      //    do nothing.
      //
      // Solution:
      //
      //  - Patch in a HandleMouseUp handler that recognizes the exit and auto-attempt 
      //    buttons.
      //
      //  - Have that handler also recognize Tile ID #9001 as a minigame click.
      //
      //  - No-op HandleNavigationInput.
      //
      // Of course, we also want to patch keyboard navigation into the minigame, so we 
      // don't just no-op HandleNavigationInput; we replace it entirely.
      //
      namespace MinigameKeynav {
         static bool s_userPressedUp = false;
         //
         namespace XAxis {
            bool __stdcall Inner(RE::LockPickMenu* menu, UInt32 direction) {
               s_userPressedUp = false;
               //
               SInt32 index = menu->unk168;
               if (index < 0)
                  return false;
               switch (direction) {
                  case RE::Menu::NavigationInput::kNavInput_Left:
                     index--;
                     break;
                  case RE::Menu::NavigationInput::kNavInput_Right:
                     index++;
                     break;
                  case RE::Menu::NavigationInput::kNavInput_Up:
                     s_userPressedUp = true;
                     return true;
                  case RE::Menu::NavigationInput::kNavInput_Down:
                  default:
                     return false;
               }
               if (index < 0 || index > 4)
                  return false;
               float center = CALL_MEMBER_FN(menu->tileBackground, GetFloatTraitValue)(kTileValue_width) / 5;
               center *= (float)index + 0.5;
               center += CALL_MEMBER_FN(menu->tileBackground, GetFloatTraitValue)(kTileValue_x);
               auto ui = RE::InterfaceManager::GetInstance();
               ui->SetCursorPosition(center, CALL_MEMBER_FN(ui, GetCursorScreenY)(), false);
               return true;
            };
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, dword ptr [esp + 0x4];
                  push eax;
                  push ecx;
                  call Inner;
                  retn 8;
               };
            };
            void Apply() {
               WriteRelJump(0x005B0D20, (UInt32)&Outer);
               SafeWrite16 (0x005B0D25, 0x9090); // courtesy NOPs
            }
         }
         namespace YAxis {
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x00403190; // OSInputGlobals::GetMouseAxisMovement
                  call eax;             // reproduce patched-over call
                  cmp  s_userPressedUp, 0;
                  je   lExit;
                  mov  eax, -4;
                  mov  s_userPressedUp, 0; // we need to reset it so that it doesn't stay true until the next navigation input
               lExit:
                  mov  ebx, 0x005AFAA0;
                  jmp  ebx;
               };
            };
            void Apply() {
               WriteRelJump(0x005AFA9B, (UInt32)&Outer);
            };
         }
         void Apply() {
            XAxis::Apply();
            YAxis::Apply();
         };
      };
      namespace GamepadMappingFixes {
         __declspec(naked) void __stdcall HandleMouseUp(SInt32 id, RE::Tile* tile) {
            _asm {
               mov eax, dword ptr [esp + 0x4];
               cmp eax, 2; // exit button
               je  lForward;
               cmp eax, 5; // auto-attempt button
               je  lForward;
               cmp eax, 9001; // HACK: minigame click button
               jne lExit;
            lForward:
               mov eax, dword ptr [ecx];
               mov eax, dword ptr [eax + 0x8];
               jmp eax; // return this->HandleMouseDown(id, tile);
            lExit:
               retn 8;
            };
         };
         void Apply() {
            SafeWrite32(RE::LockPickMenu::kVTBL + 0xC, (UInt32)&HandleMouseUp); // add a HandleMouseUp handler
         };
      };
      void Apply() {
         MinigameKeynav::Apply();
         GamepadMappingFixes::Apply();
      };
   };
};