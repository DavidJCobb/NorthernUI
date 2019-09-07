#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class BookMenu : public RE::Menu { // sizeof == 0x3C
      public:
         enum {
            kID   = 0x402,
            kVTBL = 0x00A6B334,
         };
         enum {
            kTileID_ButtonExit = 31,
            kTileID_ButtonTake = 32,
         };

         Tile*  unk28 = nullptr; // 28 // these two tiles are the scrollbar and its thumb
         Tile*  unk2C = nullptr; // 2C
         UInt32 unk30;
         UInt32 unk34;
         float  unk38; // 38 // == unk2C Y-position minus cursor Y-position, given (0, 0) is the upper-left corner?

         MEMBER_FN_PREFIX(BookMenu);
         DEFINE_MEMBER_FN(Exit, void, 0x00595F30);
   };
};