#include "StatsMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace StatsMenu {
      namespace MiscStatListindexFix {
         //
         // When Bethesda renders the misc stats, they produce blank lines between 
         // some groups of stats by skipping listindex values, NOT by rendering 
         // blank rows. Because they don't render blank rows, you can't use the 
         // childcount trait on the container to set up a scrollbar properly. You 
         // have to either hardcode the pane height... or patch the game.
         //
         // This patch writes the last generated listindex to the root tile's 
         // user24 trait.
         //
         void Inner(RE::Menu* menu, SInt32 listindex) {
            if (listindex > CALL_MEMBER_FN(menu->tile, GetFloatTraitValue)(RE::kTagID_user24))
               CALL_MEMBER_FN(menu->tile, UpdateFloat)(RE::kTagID_user24, listindex);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0058CEB0; // reproduce patched-over call to Tile::UpdateFloat
               call eax;
               mov  eax, dword ptr [esp + 0x20];
               push eax;
               push esi;
               call Inner;
               add  esp, 8;
               mov  eax, 0x005DBA7B;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005DBA76, (UInt32)&Outer); // patches StatsMenu::RenderTab5
         };
      };
      void Apply() {
         MiscStatListindexFix::Apply();
      };
   };
};