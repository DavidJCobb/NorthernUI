#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class NegotiateMenu : public Menu {
      public:
         NegotiateMenu();
         ~NegotiateMenu();

         enum {
            kTileID_Unk01 = 1,
            kTileID_MerchantName = 2,
            kTileID_ScrollBar = 3,
            kTileID_ScrollThumb = 4,
            kTileID_ButtonOK = 6,
            kTileID_ButtonCancel = 7,
            kTileID_BuyPrice = 9,
            kTileID_SellPrice = 10,
            kTileID_PlayerSkill = 11,
            kTileID_MerchantDisposition = 12,
            kTileID_NorthernUI_ButtonOKShim = 9001,
         };

         Tile*  unk28            = nullptr; // 28 // ID #1
         Tile*  tileMerchantName = nullptr; // 2C // ID #2
         Tile*  tileScrollBar    = nullptr; // 30 // ID #3
         Tile*  tileScrollThumb  = nullptr; // 34 // ID #4
         Tile*  tileButtonOK     = nullptr; // 38 // ID #6
         Tile*  tileButtonCancel = nullptr; // 3C // ID #7
         Tile*  tileBuyPrice     = nullptr; // 40 // ID #9
         Tile*  tileSellPrice    = nullptr; // 44 // ID#10
         Tile*  tilePlayerSkill;                   // 48 // ID#11 // this pointer isn't initialized when the menu is created!
         Tile*  tileMerchantDisposition = nullptr; // 4C // ID#12
         UInt32 unk50;
         UInt32 unk54 = 1; // 54

   };
};