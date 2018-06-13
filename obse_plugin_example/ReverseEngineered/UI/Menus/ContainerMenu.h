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

         Tile*  scrollBar;		// 28
         Tile*  scrollMarker;		// 2C
         Tile*  listContents;		// 30
         Tile*  focusBox;			// 34
         Tile*  invP4P5Header;	// 38
         Tile*  selectedItemTile;	// 3C // user11 = idx of item in inventory/container
         UInt8  filterType;			// 40
         UInt8  pad41[3];
         TESObjectREFR* refr; // 44
         UInt32 unk48;				// 48
         UInt32 unk4C;				// 4C
         float  unk50;				// 50
         UInt8  unk54;				// 54
         UInt8  bTransactionInProgress;	// set true after first transaction confirmed by player. ###TODO: reset when?
         UInt8  unk56;				// may be padding
         UInt8  pad57;
         UInt32 unk58;				// 58
         UInt32 unk5C;				// 5C
         UInt8  unk60;				// 60
         bool   isBarter;			//	 1 if bartering with merchant
         UInt8  unk62;
         UInt8  unk63;
         bool   isContainerContents; //init'd to 1. 0 when switched to player's inventory view
         UInt8  pad65[3];

         UInt32		GetItemIndex();
         TESForm*	GetItem();

         static UInt32 GetQuantity();
         static void Update();
   };
};
