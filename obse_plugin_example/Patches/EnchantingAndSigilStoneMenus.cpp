#include "EnchantingAndSigilStoneMenus.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameMenus.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace EnchantingAndSigilStoneMenus {
      namespace RepairMenuAware {
         void Inner(RE::Menu* menu) {
            auto tileRepair = (RE::Tile*) g_TileMenuArray->data[kMenuType_Repair - kMenuType_Message];
            bool repairOpen = false;
            if (tileRepair && CALL_MEMBER_FN(tileRepair, GetFloatTraitValue)(kTileValue_visible) != 1.0F)
               repairOpen = true;
            //
            CALL_MEMBER_FN((RE::Tile*) menu->tile, UpdateFloat)(kTileValue_user24, repairOpen ? 2.0F : 1.0F);
         };
         __declspec(naked) void OuterEnchanting() {
            _asm {
               push ecx; // protect
               push esi;
               call Inner;
               add  esp, 4;
               pop  ecx; // restore
               mov  eax, 0x0057D2F0; // patched-over call
               call eax;
               mov  ecx, 0x005A210E;
               jmp  ecx;
            };
         };
         __declspec(naked) void OuterSigilStone() {
            _asm {
               push ecx; // protect
               push esi;
               call Inner;
               add  esp, 4;
               pop  ecx; // restore
               mov  eax, 0x0057D2F0; // patched-over call
               call eax;
               mov  ecx, 0x005D55D8;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x005A2109, (UInt32)&OuterEnchanting);
            WriteRelJump(0x005D55D3, (UInt32)&OuterSigilStone);
         };
      };

      void Apply() {
         RepairMenuAware::Apply();
      };
   };
};