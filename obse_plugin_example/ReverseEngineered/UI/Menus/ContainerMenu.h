#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class ContainerMenu : public Menu { // sizeof == 0x68
      public:
         ContainerMenu();
         ~ContainerMenu();

         enum {
            kContValue_CurrentTab = kTileValue_user0,
            kContValue_WeaponHeaderPos,
            kContValue_ApparelHeaderPos,
            kContValue_AlchemyHeaderPos,
            kContValue_MiscHeaderPos,
            kContValue_NumItemsInList,
            kContValue_Gold,
            kContValue_IsContainerMode,			//false if looking at inventory
            kContValue_CanTakeAll,				//false if "Take All" button hidden
            kContValue_CanNegotiate,
            kContValue_MagicPopupXPos,
            kContValue_NPCName,
            kContValue_BarterGoldBase,
            kContValue_CurrentEncumbrance,
            kContValue_MaxEncumbrance,
         };
         enum {
            kTileID_ListItem = 0x33, // ID#51
         };

         Tile*  scrollBar        = nullptr; // 28
         Tile*  scrollMarker     = nullptr; // 2C
         Tile*  listContents     = nullptr;		// 30
         Tile*  focusBox         = nullptr;			// 34
         Tile*  invP4P5Header    = nullptr;	// 38
         Tile*  selectedItemTile = nullptr;	// 3C // user11 = idx of item in inventory/container
         UInt8  filterType; // 40 // SKSE definition may be wrong. This is initialized as a UInt32, and to 0xF.
         UInt8  pad41[3];
         TESObjectREFR* refr = nullptr; // 44
         SInt32 listScrollPosPlayer = 0; // 48 // the lists use the same scrollbar; we cache and restore scroll positions when swapping
         SInt32 listScrollPosRefr   = 0; // 4C
         float  unk50 = 0.0F;  // 50
         bool   quantityMenuInProgress = false; // 54 // set to true while waiting for the quantity menu to be answered
         bool   transactionInProgress  = false; // 55 // set true after first transaction confirmed by player. ###TODO: reset when?
         UInt8  unk56 = 0;    // 56
         UInt8  pad57;
         UInt32 unk58;				// 58
         UInt32 unk5C;				// 5C
         UInt8  unk60    = 0;     // 60
         bool   isBarter = false; // 61
         UInt8  unk62    = 0;     // 62
         UInt8  unk63;
         bool   isContainerContents = true; // false when switched to player's inventory view
         UInt8  pad65[3];

         UInt32   GetItemIndex();
         TESForm* GetItem();

         static UInt32 GetQuantity();
         static void   Update();
   };
};
