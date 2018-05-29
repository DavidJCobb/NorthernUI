#include "OptionsMenu.h"
#include "obse_common/SafeWrite.h"

#include "Patches/NewMenus/XXNOptionsMenu.h"
#include "Patches/NewMenus/XXNControlsMenu.h"

namespace CobbPatches {
   namespace OptionsMenu {
      namespace NewSubmenus {
         //
         // Add options submenus for NorthernUI.
         //
         struct Shim : public RE::Menu {
            void ExtendedSubmenus(SInt32 tileID, RE::Tile* tile) {
               switch (tileID) {
                  case 9001:
                     CALL_MEMBER_FN(this, FadeOut)();
                     ShowXXNOptionsMenu();
                     break;
                  case 9002:
                     CALL_MEMBER_FN(this, FadeOut)();
                     ShowXXNControlsMenu();
                     break;
               }
            };
         };
         __declspec(naked) void Outer() {
            _asm {
               // in OptionsMenu::HandleMouseUp
               // eax == Arg1
               // ecx == this
               cmp  eax, 8;
               jne  lExtended;
               mov  eax, 0x005BD92B;
               jmp  eax;
            lExtended:
               jmp  Shim::ExtendedSubmenus;
            };
         };
         void Apply() {
            WriteRelJump(0x005BD926, (UInt32)&Outer); // patch OptionsMenu::HandleMouseUp+0xA6
         };
      };
      //
      void Apply() {
         NewSubmenus::Apply();
      };
   };
};
