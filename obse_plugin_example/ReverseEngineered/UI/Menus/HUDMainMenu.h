#pragma once
#include "shared.h"
#include "ReverseEngineered/UI/Menu.h"

namespace RE {
   class HUDMainMenu : public Menu {
      public:
         Tile*  tileBackground; // 28 // ID #1
         Tile*  tileHealthBar;  // 2C // ID #2
         Tile*  tileMagickaBar; // 30 // ID #3
         Tile*  tileStaminaBar; // 34 // ID #4
         Tile*  tileEquippedWeapon; // 38 // ID #5
         Tile*  tileEquippedSpell;  // 3C // ID #6
         Tile*  tileCompassWindow;  // 40 // ID #7
         Tile*  tileCompassHeading; // 44 // ID #14
         Tile*  tileFrame;    // 48 // ID #8
         Tile*  tileGrabZone; // 4C // ID #91 // Tile used for clicking and dragging the player model in the Big Four menus.
         Tile*  tileActiveEffects; // 60 // ID #16
         Tile*  tileLevelUpIcon;   // 54 // ID #18
         Tile*  tileBrackets; // 58 // ID #20
         UInt32 unk5C; // 5C
         UInt32 unk60; // 60
         UInt32 unk64; // 64
         float  cursorDragX; // 68
         float  cursorDragY; // 6C
         float  unk70; // 70
         // ...?
   };
};