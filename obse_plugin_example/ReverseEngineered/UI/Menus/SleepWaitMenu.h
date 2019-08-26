#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class SleepWaitMenu : public RE::Menu { // sizeof == 0x50
      public:
      enum {
         kID   = 0x3F4,
         kVTBL = 0x00A6D7C4,
      };
      enum {
         kTileID_Background   = 1,
         kTileID_PromptText   = 2,
         kTileID_SliderBase   = 3,
         kTileID_SliderThumb  = 4,
         kTileID_ButtonSleep  = 5,
         kTileID_ButtonCancel = 6,
         kTileID_TextWeekdayAndTime =  8,
         kTileID_TextDate           =  9,
         kTileID_SliderMouseBlocker = 10,
         kTileID_SliderKeynavShim   = 11,
      };
      enum {
         kRootTrait_IsForSleeping = kTagID_user0,
         kRootTrait_NorthernUI_UseCustomFormat = kTagID_user22,
         kRootTrait_NorthernUI_CustomFormat01  = kTagID_user23,
         kRootTrait_NorthernUI_CustomFormat02  = kTagID_user24,
      };

      Tile* tileSliderBase;   // 28 // ID #3
      Tile* tileSliderThumb;  // 2C // ID #4
      Tile* tileButtonSleep;  // 30 // ID #5
      Tile* tileButtonCancel; // 34 // ID #6
      Tile* tileTextTime;     // 38 // ID #8
      Tile* tileTextDate;     // 3C // ID #9
      Tile* tileSliderMouseBlocker; // 40 // ID#10
      Tile* tileSliderKeynavShim;   // 44 // ID#11
      UInt32 unk48;
      UInt8  unk4C;
      UInt8  unk4D[3];

      MEMBER_FN_PREFIX(SleepWaitMenu);
      DEFINE_MEMBER_FN(AllNotableTilesExist, bool, 0x005D68A0);
   };

   DEFINE_SUBROUTINE_EXTERN(bool, ShowSleepWaitMenu, 0x005D6D20); // handles text formatting for menu; format strings are hardcoded

   DEFINE_SUBROUTINE_EXTERN(bool, OnSleepWaitMenuFrame, 0x005D7090); // called by the main loop on every frame the menu is open; do not call directly
   DEFINE_SUBROUTINE_EXTERN(void, OnSleepWaitDone, 0x005D6A10); // called by above
};