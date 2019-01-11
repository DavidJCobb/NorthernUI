#include "Patches/DialogMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/DialogMenu.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace DialogMenu {
      namespace EnhancedEconomyCompat {
         //
         // Enhanced Economy tries to send clicks to certain tiles. The menu system doesn't 
         // allow you to shim clicks, and we can't rearrange our menu structure so that the 
         // tile names line up, so instead, we need to catch clicks to a shim tile and 
         // forward them to the real tile via the DLL. We use new tile IDs starting at 9001 
         // for this purpose.
         //
         void Inner(RE::DialogMenu* menu) {
            menu->HandleMouseUp(RE::DialogMenu::kTileID_ButtonBarter, menu->tileButtonBarter);
         }
         __declspec(naked) void Outer() {
            _asm {
               cmp  eax, 9001;
               jne  lNormal;
               push esi;
               call Inner;
               add  esp, 4;
               mov  eax, 0x0059F636; // returns
               jmp  eax;
            lNormal:
               cmp  eax, 0x64;
               jl   lLess;
               mov  eax, 0x0059F023;
               jmp  eax;
            lLess:
               mov  ecx, 0x0059F0D6;
               jmp  ecx;
            };
         }
         void Apply() {
            WriteRelJump(0x0059F01D, (UInt32)&Outer);
            SafeWrite8  (0x0059F022, 0x90); // NOP
         }
      }
      namespace FullwidthAutoSwitch {
         //
         // NorthernUI's dialogue menu design doesn't allow for long topics... We can fix this 
         // by using C++ to check topics' rendered widths and inform the XML of the largest one.
         //
         enum {
            kTrait_LargestWidth = RE::kTagID_user24,
         };
         static bool s_needsUpdate = false;
         //
         namespace OnTopicsRedrawn {
            void _stdcall Inner(RE::DialogMenu* menu) {
               s_needsUpdate = true;
            }
            __declspec(naked) void Outer() {
               _asm {
                  mov  eax, 0x006B85C0; //
                  call eax;             // reproduce patched-over call
                  push edi;
                  call Inner; // stdcall
                  mov  eax, 0x0059E9C3;
                  jmp  eax;
               }
            }
            void Apply() {
               WriteRelJump(0x0059E9BE, (UInt32)&Outer); // DialogMenu::RenderTopics
            }
         }
         namespace OnFrame {
            void _stdcall Inner(RE::DialogMenu* menu) {
               if (s_needsUpdate) {
                  auto root = menu->tile;
                  auto list = menu->tileTopicScrollPane;
                  _MESSAGE("DialogMenu hook!");
                  if (!list)
                     return;
                  _MESSAGE(" - List pane exists");
                  float largest = 0.0F;
                  for (auto node = list->childList.start; node; node = node->next) {
                     auto tile = node->data;
                     if (!tile)
                        continue;
                     float width = CALL_MEMBER_FN(tile, GetFloatTraitValue)(RE::kTagID_width);
                     _MESSAGE(" - Tile has width %f", width);
                     if (width > largest)
                        largest = width;
                  }
                  _MESSAGE(" - Largest width was %f", largest);
                  CALL_MEMBER_FN(root, UpdateFloat)(kTrait_LargestWidth, largest);
                  //
                  s_needsUpdate = false;
               }
            }
            __declspec(naked) void Outer() {
               _asm {
                  push esi;
                  call Inner; // stdcall
                  mov  eax, 0x00578FE0; // 
                  call eax;             // reproduce patched-over call
                  mov  ecx, 0x0059F660;
                  jmp  ecx;
               }
            }
            void Apply() {
               WriteRelJump(0x0059F65B, (UInt32)&Outer); // DialogMenu::HandleFrame
            }
         }
         void Apply() {
            OnTopicsRedrawn::Apply();
            OnFrame::Apply();
         }
      }
      //
      void Apply() {
         EnhancedEconomyCompat::Apply();
         FullwidthAutoSwitch::Apply();
      };
   };
};