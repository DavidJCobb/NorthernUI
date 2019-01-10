#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class MessageMenu : public RE::Menu { // sizeof == 0x64
      public:
         enum {
            kID   = 0x3E9,
            kVTBL = 0x00A6CE5C,
         };
         enum {
            kTileID_Background  = 1,
            kTileID_MessageText = 2,
            kTileID_FocusBox    = 3,
         };
         RE::Tile* tileBackground; // 28
         RE::Tile* tileMessageText; // 2C
         RE::Tile* tileFocusBox; // 30
         RE::Tile* tileButtons[10]; // 34 - 58
         UInt32    unk5C = 0;
         UInt8     unk60 = 1;
         UInt8     pad61[3];
   };
};