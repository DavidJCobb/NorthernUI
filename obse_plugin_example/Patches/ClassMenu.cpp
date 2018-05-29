#include "Patches/ClassMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace ClassMenu {
      namespace DisableHardcodedXboxMappings {
         //
         // ClassMenu hardcodes Xbox button mappings for the A, B, and X buttons. This 
         // code disables those mappings if the root tile's USER24 trait is &true;.
         //
         bool Inner(RE::Menu* menu) {
            auto tile = (RE::Tile*) menu->tile;
            if (!tile)
               return true;
            return CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_user24) != 2.0F;
         };
         __declspec(naked) void Outer() {
            _asm {
               push esi;
               call Inner;
               add  esp, 4;
               test al, al;
               mov  ecx, 0x005969F6; // jump to original subroutine's "return false" code
               jz   lExit;
               mov  eax, 0x00578FE0; // patched-over call: GetTopmostMenuIDUnderUnkCondition
               call eax;
               mov  ecx, 0x00596992;
            lExit:
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x0059698D, (UInt32)&Outer);
         };
      };

      void Apply() {
         DisableHardcodedXboxMappings::Apply();
      };
   };
};