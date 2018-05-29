#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameExtraData.h"
#include "obse/GameMenus.h"

namespace RE {
   class RepairMenu : public Menu { // sizeof == 0x78
      public:
		   enum { kID = 0x40B };
         struct RepairItemAndIndex { // sizeof == 0x8
            ExtraContainerChanges::EntryData* data;
            UInt32                            index; // index in the inventory (since indices aren't, like, persistent)
         };
         typedef NiTListBase<RepairItemAndIndex> RepairMenuList; // This is how Bethesda subclassed/typedef'd it

         enum MenuOpener : UInt32 {
            kMenuOpener_Repair                 = 1,
            kMenuOpener_RepairBuy              = 2, // paying a merchant to repair your crap
            kMenuOpener_AlchemyMenu            = 3,
            kMenuOpener_EncantmentMenu_Enchant = 4,
            kMenuOpener_EncantmentMenu_SoulGem = 5,
            kMenuOpener_SigilStoneMenu         = 6,
         };

         Tile*  unkID01;         // 28 // ID #1 // not used in vanilla
         Tile*  buttonClose;     // 2C // ID #2
         Tile*  buttonRepairAll; // 30 // ID#16
         Tile*  supplyCounter;   // 34 // ID #3 // amount of gold or repair hammers available
         Tile*  listScrollbar;   // 38 // ID #5
         Tile*  focusBox;        // 3C // ID #7
         Tile*  listScrollThumb; // 40 // ID #6
         Tile*  listContents;    // 44 // ID #8
         Tile*  unkID11;         // 48 // ID#11 // not used in vanilla
         Tile*  buttonRemove;    // 4C // ID#15 // alchemy: selecting an already-picked ingredient opens this menu and shows this button; this menu reaches directly into AlchemyMenu state to do the relevant checks
         Tile*  buttonFilter;    // 50 // ID#17
         UInt32         unk54;
         MenuOpener     opener = kMenuOpener_Repair; // 58
         UInt32         unk5C;
         UInt32         unk60;
         UInt8          unk64 = 0; // 64 // set to 1 while we're waiting for the player to respond to the "Repair All" confirmation prompt
         UInt8          isFiltering = false; // 65 // only valid for alchemy ingredients; filters by effect type
         UInt16         pad66;     // 66
         RepairMenuList unk68;     // 68
   };
   static_assert(sizeof(RepairMenu) <= 0x78, "RE::RepairMenu is too large!");
   static_assert(sizeof(RepairMenu) >= 0x78, "RE::RepairMenu is too small!");

   DEFINE_SUBROUTINE_EXTERN(void, RepairMenu_RepairAllConfirmCallback, 0x005D1FC0);
};
