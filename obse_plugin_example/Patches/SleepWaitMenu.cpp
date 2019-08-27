#include "SleepWaitMenu.h"
#include "ReverseEngineered\UI\Menus\SleepWaitMenu.h"
#include "Miscellaneous/InGameDate.h"

#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace SleepWaitMenu {
      namespace CustomDateTimeFormat {
         void _stdcall Inner(RE::SleepWaitMenu* menu) {
_MESSAGE("ATTEMPTING TO FORMAT SLEEPWAITMENU... menu pointer is %08X", menu);
            auto root = menu->tile;
            if (CALL_MEMBER_FN(root, GetFloatTraitValue)(menu->kRootTrait_NorthernUI_UseCustomFormat) != (float)RE::kEntityID_true)
               return;
            const char* format01 = CALL_MEMBER_FN(root, GetStringTraitValue)(menu->kRootTrait_NorthernUI_CustomFormat01);
            const char* format02 = CALL_MEMBER_FN(root, GetStringTraitValue)(menu->kRootTrait_NorthernUI_CustomFormat02);
            bool use01 = format01 && format01[0] != '\0';
            bool use02 = format02 && format02[0] != '\0';
            if (!use01 && !use02)
               return;
            InGameDate date;
            date.SetFromTime(RE::g_timeGlobals);
            char formatted01[256];
            char formatted02[256];
            date.Format(formatted01, format01);
            date.Format(formatted02, format02);
            CALL_MEMBER_FN(menu->tileTextTime, UpdateString)(RE::kTagID_string, formatted01);
            CALL_MEMBER_FN(menu->tileTextDate, UpdateString)(RE::kTagID_string, formatted02);
         }
         //
         namespace OnShow {
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x0057DE50; // reproduce patched-over call to PlayUIClicksound
                  call eax;             //
                  // PlayUIClicksound is non-member but don't clean up; we're jumping back 
                  // to its ADD ESP, 4 line after we run our code.
                  push esi;
                  call Inner; // stdcall
                  mov  eax, 0x005D703F;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x005D703A, (UInt32)&Outer);
            }
         }
         namespace DuringWait {
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x0058CED0; // reproduce patched-over call to Tile::UpdateString
                  call eax;             //
                  push esi;
                  call Inner; // stdcall
                  mov  eax, 0x005D7392;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x005D738D, (UInt32)&Outer);
            }
         }
         void Apply() {
            OnShow::Apply();
            DuringWait::Apply();
         }
      }
      namespace FixSoftlockOnCancel {
         //
         // On every frame, the game's main loop checks if SleepWaitMenu is open; 
         // if so, it calls OnSleepWaitMenuFrame, which is what allows the menu to 
         // tick down while you're waiting. If OnSleepWaitMenuFrame determines that 
         // the wait is over, it calls OnSleepWaitDone, which attempts to close the 
         // menu using Menu::FadeOut.
         //
         // This is vulnerable to a softlock, which without this patch can be caused 
         // with the following steps:
         //
         //    1. Start waiting.
         //    2. Move the mouse.
         //    3. While the wait is ticking down and the mouse is moving, press 
         //       B on the gamepad to cancel the wait early.
         //
         // Follow those steps, and you'll find yourself trapped on a black screen, 
         // with the cursor moving and a UI clicksound playing on an endless loop.
         //
         // Here's the cause:
         //
         // When you click the Cancel button in SleepWaitMenu, the menu receives a 
         // click event specifying the ID of the cancel button, and a pointer to 
         // the tile that generated the click event. Pay careful attention to my 
         // wording there: I said "the tile that generated the click event." If 
         // keyboard navigation sends a click to the cancel button, the menu will 
         // receive a click event indicating the tile whose keyboard navigation 
         // trait was processed.
         //
         // When SleepWaitMenu determines (based on the tile ID) that the click 
         // was on the cancel button, it attempts to hide the tile that it received 
         // as an argument, and then calls OnSleepWaitDone. This can lead to a soft-
         // lock.
         //
         // If you define keyboard navigation on the root tile and use that to 
         // close the menu, then the menu's root tile will be forcibly hidden. 
         // However, this prevents Menu::FadeOut from doing anything (it only 
         // acts on menus that are visible), which means that SleepWaitMenu cannot 
         // close.
         //
         // But why does this only softlock if the cursor is being moved? I don't 
         // know. Perhaps moving the cursor alters some aspect of how HandleMouseUp 
         // is called.
         //
         __declspec(naked) void Outer() {
            _asm {
               mov  ecx, dword ptr [esi + 0x34];
               push 0xFA1;
               mov  eax, 0x005D6B71;
               jmp  eax;
            }
         }
         void Apply() {
            WriteRelJump(0x005D6B6C, (UInt32)&Outer);
         }
      }
      //
      void Apply() {
         CustomDateTimeFormat::Apply();
         FixSoftlockOnCancel::Apply();
      }
   };
};