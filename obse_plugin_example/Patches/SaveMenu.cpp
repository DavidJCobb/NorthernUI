#include "SaveMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
//
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace SaveMenu {
      namespace FixRootUser0 {
         //
         // The root tile's user0 is supposed to indicate whether the "New Save" list item 
         // has focus. However, the trait is never set.
         //
         void Inner(RE::Menu* menu, bool isNewSave) {
            CALL_MEMBER_FN((RE::Tile*)menu->tile, UpdateFloat)(kTileValue_user0, isNewSave ? 1.0F : 2.0F);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0058CEB0; // reproduce patched-over call
               call eax;
               xor  eax, eax;
               cmp  ebx, 0x65;
               sete al;
               push eax;
               push esi;
               call Inner;
               add  esp, 8;
            lExit:
               mov  eax, 0x005D3597;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005D3592, (UInt32)&Outer);
         };
      };

      void Apply() {
         FixRootUser0::Apply();
      };
   };
};