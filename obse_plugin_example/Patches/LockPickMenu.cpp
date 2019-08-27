#include "LockPickMenu.h"
#include "obse_common/SafeWrite.h"

#include "XboxGamepad/Main.h"
#include "XboxGamepad/Patch.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/LockPickMenu.h"

#include "ReverseEngineered/Systems/Timing.h"
#include "Miscellaneous/strings.h"

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
      constexpr bool ce_handleAButtonOnRelease = false;
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
                  case RE::InterfaceManager::kNavigationKeypress_XboxA:
                     if (ce_handleAButtonOnRelease) {
                        menu->HandleMouseDown(0, nullptr);
                        return true;
                     } else
                        return false;
                  case RE::InterfaceManager::kNavigationKeypress_Left:
                     index--;
                     break;
                  case RE::InterfaceManager::kNavigationKeypress_Right:
                     index++;
                     break;
                  case RE::InterfaceManager::kNavigationKeypress_Up:
                     s_userPressedUp = true;
                     return true;
                  case RE::InterfaceManager::kNavigationKeypress_Down:
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
                  call Inner; // stdcall
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
      }
      namespace Debugging {
         void _stdcall Inner(RE::LockPickMenu* menu) {
            RE::Tile* tiles[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
            tiles[0] = RE::GetDescendantTileByName(menu->tile, "northernUI_debug_tumbler_1");
            tiles[1] = RE::GetDescendantTileByName(menu->tile, "northernUI_debug_tumbler_2");
            tiles[2] = RE::GetDescendantTileByName(menu->tile, "northernUI_debug_tumbler_3");
            tiles[3] = RE::GetDescendantTileByName(menu->tile, "northernUI_debug_tumbler_4");
            tiles[4] = RE::GetDescendantTileByName(menu->tile, "northernUI_debug_tumbler_5");
            for (UInt32 i = 0; i < 5; i++) {
               auto tile = tiles[i];
               if (!tile)
                  continue;
               auto& tumbler = menu->tumblers[i];
               //
               std::string unk04;
               {
                  if (tumbler.hangStart == -1) {
                     unk04 = "<NONE>";
                  } else {
                     UInt32 time = RE::g_timeInfo->unk10 - tumbler.hangStart;
                     UInt32 seconds = time / 1000;
                     UInt32 milli = time % 1000;
                     cobb::snprintf(unk04, "%c%02d.%03ds", time < 0 ? '-' : ' ', seconds, milli);
                  }
               }
               std::string hangTime;
               {
                  UInt32 time    = tumbler.hangTime;
                  UInt32 seconds = time / 1000;
                  UInt32 milli   = time % 1000;
                  cobb::snprintf(hangTime, "%c%02d.%03ds", time < 0 ? '-' : ' ', seconds, milli);
               }
               //
               std::string output;
               cobb::snprintf(output,
                  "TUMBLER %d\nOffset: %f\Hang timer: %s\nHang duration: %s\nUnk0C: %f\nUnk10: %f\nVelocity: %f\nMoving: %d\nSolved: %d\nUnk1A: %d\n",
                  i,
                  tumbler.heightOffset,
                  unk04.c_str(),
                  hangTime.c_str(),
                  tumbler.unk0C,
                  tumbler.unk10,
                  tumbler.velocity,
                  tumbler.isMoving,
                  tumbler.isSolved,
                  tumbler.unk1A
               );
               CALL_MEMBER_FN(tile, UpdateString)(RE::kTagID_string, output.c_str());
            }
         }
         __declspec(naked) void Outer() {
            _asm {
               push esi;
               call Inner; // stdcall
               mov  eax, 0x0057C140; // reproduce patched-over call to MenuModeHasFocus
               call eax;
               mov  ecx, 0x005B11B0;
               jmp  ecx;
            }
         }
         void Apply() {
            WriteRelJump(0x005B11AB, (UInt32)&Outer);
         }
      }
      //
      void Apply() {
         MinigameKeynav::Apply();
         if (!ce_handleAButtonOnRelease)
            GamepadMappingFixes::Apply();
         Debugging::Apply();
      };
   };
};