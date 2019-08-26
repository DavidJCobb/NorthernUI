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
      //
      void Apply() {
         CustomDateTimeFormat::Apply();
      }
   };
};