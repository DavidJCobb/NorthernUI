#pragma once
#include "shared.h"
#include "ReverseEngineered/UI/Menu.h"

namespace RE {
   class HUDInfoMenu : public Menu { // sizeof == 0x5C
      public:
         TileText* name;        // 28 // ID #1
         TileText* value;       // 2C // ID #2
         TileText* weight;      // 30 // ID #3
         TileText* damage;      // 34 // ID #4
         TileText* armor;       // 38 // ID #5
         TileText* quality;     // 3C // ID #6 // apparatus quality
         TileText* condition;   // 40 // ID #7 // armor/weapon condition
         TileText* uses;        // 44 // ID #8
         TileText* destination; // 48 // ID #9 // destination of load door
         TileText* lock;        // 4C // ID#10 // lock strength
         Tile*     actionIcon;  // 50 // ID#11 // Xbox action icon
         TESObjectREFR* target; // 54 // probably current reticle focus
         UInt32         unk58;  // 58 // counter; starts at zero; when it hits 60, it gets reset and something is done with unk54 if it's not null
   };

   DEFINE_SUBROUTINE_EXTERN(bool, UpdateHUDInfoMenuTarget, 0x005A4980, TESObjectREFR* target, bool telekinesis, UInt32 action);
};