#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/Menu.h"

namespace RE {
   class OptionsMenu : public Menu { // sizeof == 0x40
      public:
         enum {
            kTileID_ButtonExit     = 2,
            kTileID_ButtonGameplay = 5,
            kTileID_ButtonVideo    = 6,
            kTileID_ButtonAudio    = 7,
            kTileID_ButtonControls = 8,
            kTIleID_ButtonDLC      = 9,
            kTileID_ButtonNorthernUIOptions  = 9001,
            kTileID_ButtoNNorthernUIControls = 9002,
         };

         Tile* tileButtonExit     = nullptr; // 28 // tile ID #2
         Tile* tileButtonGameplay = nullptr; // 2C // tile ID #5
         Tile* tileButtonVideo    = nullptr; // 30 // tile ID #6
         Tile* tileButtonAudio    = nullptr; // 34 // tile ID #7
         Tile* tileButtonControls = nullptr; // 38 // tile ID #8
         Tile* tileButtonDLC      = nullptr; // 3C // tile ID #9
   };

   DEFINE_SUBROUTINE_EXTERN(void, OnOptionsSubmenuClose, 0x005BD610);
};