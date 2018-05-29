#pragma once
#include "ReverseEngineered/_BASE.h"
#include "obse/GameExtraData.h"
#include "obse/GameMenus.h"

namespace RE {
   class InventoryMenu : public Menu { // sizeof == 0x58
      public:
         InventoryMenu();
         ~InventoryMenu();

         TileRect*  focusBox;			//028
         TileRect*  listContents;		//02C
         TileRect*  scrollBar;		//030
         TileImage* scrollMarker;		//034
         TileRect*  invP4P5Header;	//038 - ?
         UInt32			unk3C;				//03C
         UInt8			filterType;			//040 init'd to 1F (all), 1=weapons, 2=armor, ...
         UInt8			pad41[3];
         UInt8			unk44;				//044 init'd to FF
         UInt8			unk45[3];
         float			unk48;				//048
         UInt32			unk4C;				//04C
         ExtraContainerChanges::EntryData* unk50; // 50
         UInt32			unk54;				//054 uninitialized
   };
};