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
         };
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
               jle  lLess;
               mov  eax, 0x0059F023;
               jmp  eax;
            lLess:
               mov  ecx, 0x0059F0D6;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x0059F01D, (UInt32)&Outer);
            SafeWrite8  (0x0059F022, 0x90); // NOP
         };
      };

      void Apply() {
         EnhancedEconomyCompat::Apply();
      };
   };
};