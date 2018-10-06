#include "Patches/NegotiateMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/NegotiateMenu.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace NegotiateMenu {
      namespace EnhancedEconomyCompat {
         //
         // Enhanced Economy tries to send clicks to certain tiles. The menu system doesn't 
         // allow you to shim clicks, and we can't rearrange our menu structure so that the 
         // tile names line up, so instead, we need to catch clicks to a shim tile and 
         // forward them to the real tile via the DLL. We use new tile IDs starting at 9001 
         // for this purpose.
         //
         void Inner(RE::NegotiateMenu* menu, SInt32 id) {
            if (id == 9001)
               menu->HandleMouseUp(RE::NegotiateMenu::kTileID_ButtonOK, menu->tileButtonOK);
         };
         __declspec(naked) void Outer() {
            _asm {
               cmp  eax, 9000;
               jl   lNormal;
               push eax;
               push esi;
               call Inner;
               add  esp, 8;
               retn; // since vanilla subroutine doesn't push any registers prior to our hook
            lNormal:
               cmp  eax, 6;
               jne  lNotEqual;
               mov  eax, 0x005BD49D;
               jmp  eax;
            lNotEqual:
               mov  ecx, 0x005BD536;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x005BD497, (UInt32)&Outer);
            SafeWrite8  (0x005BD49C, 0x90); // NOP
         };
      };

      void Apply() {
         EnhancedEconomyCompat::Apply();
      };
   };
};