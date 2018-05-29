#include "AlchemyMenu.h"
#include "obse/GameMenus.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/AlchemyMenu.h"
#include "ReverseEngineered/UI/Menus/RepairMenu.h"

#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace AlchemyMenu {
      namespace RepairMenuDataTransfer {
         //
         // Allow AlchemyMenu to pull data from the RepairMenu and MagicPopupMenu on every frame. These 
         // traits are set on AlchemyMenu's root tile:
         //
         // user16 == this handler is running
         // user17 == RepairMenu is open
         // user18 == private-use value copied from RepairMenu's root tile (if user17 is true)
         // user19 == RepairMenu is open and MagicPopupMenu is visible
         // user20 == private-use value copied from MagicPopupMenu's root tile (if user19 is true)
         //
         enum {
            kTrait_HookIsRunning    = kTileValue_user16,
            kTrait_RepairMenuOpen   = kTileValue_user17,
            kTrait_RepairMenuValue  = kTileValue_user18,
            kTrait_PopupMenuVisible = kTileValue_user19,
            kTrait_PopupMenuValue   = kTileValue_user20,
         };
         void Inner(RE::AlchemyMenu* menu) {
            if (!menu || !menu->tile)
               return;
            auto tileAlchemy = (RE::Tile*) menu->tile;
            auto tileRepair  = (RE::Tile*) g_TileMenuArray->data[kMenuType_Repair - kMenuType_Message];
            auto tileCard    = (RE::Tile*) g_TileMenuArray->data[kMenuType_MagicPopup - kMenuType_Message];
            //
            CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_HookIsRunning, 2.0F);
            if (tileRepair && CALL_MEMBER_FN(tileRepair, GetFloatTraitValue)(kTileValue_visible) != 1.0F) {
               CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_RepairMenuOpen,  2.0F);
               CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_RepairMenuValue, CALL_MEMBER_FN(tileRepair, GetFloatTraitValue)(kTrait_RepairMenuValue));
               //
               // Handle the item card. There can be single-frame moments when it's hidden, if the user is 
               // using the scroll wheel to scroll RepairMenu's inventory pane, so let's put a one-frame 
               // delay on hiding the item card details.
               //
               {
                  // Only update every thirtieth of a second. When using the scroll wheel to scroll the 
                  // RepairMenu, the MagicPopupMenu rapidly flickers (because mouseover events aren't 
                  // handled reliably when the mouse is over a scrolling pane). We can't prevent our UI 
                  // from reacting to that weirdly, but adding a thirtieth-of-a-second update interval 
                  // to item-card-related stuff helps a bit, and replicates the update interval used when 
                  // I first coded all this data-passing using scripts and OBSE script functions.
                  //
                  static UInt32 lastUpdate = 0;
                  UInt32 current = RE::g_timeInfo->unk10;
                  UInt32 elapsed = current - lastUpdate;
                  if (elapsed < 33)
                     return;
                  lastUpdate = current;
               }
               if (tileCard) {
                  CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_PopupMenuVisible, CALL_MEMBER_FN(tileCard, GetFloatTraitValue)(kTileValue_visible));
                  CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_PopupMenuValue,   CALL_MEMBER_FN(tileCard, GetFloatTraitValue)(kTrait_PopupMenuValue));
               } else {
                  CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_PopupMenuVisible, 1.0F);
                  CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_PopupMenuValue,   0.0F);
               }
            } else {
               CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_RepairMenuOpen,   1.0F);
               CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_RepairMenuValue,  0.0F);
               CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_PopupMenuVisible, 1.0F);
               CALL_MEMBER_FN(tileAlchemy, UpdateFloat)(kTrait_PopupMenuValue,   0.0F);
            }
         };
         __declspec(naked) void Outer() {
            _asm {
               push esi;      // was patched over
               mov  esi, ecx; // was patched over
               push ecx;
               call Inner;
               pop  ecx;
               mov  ecx, dword ptr [esi + 0xA0]; // was patched over
               //
               // Return to native code.
               //
               mov  eax, 0x00593AF9;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x00593AF0, (UInt32)&Outer);
            SafeWrite32 (0x00593AF5, 0x90909090); // courtesy NOPs
         };
      };

      void Apply() {
         RepairMenuDataTransfer::Apply();
      };
   };
};