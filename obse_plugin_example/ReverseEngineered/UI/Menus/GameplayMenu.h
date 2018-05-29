#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class GameplayMenu : public RE::Menu {
      public:
         enum {
            kID   = 0x3FC,
            kVTBL = 0x00A6BD0C,
         };
         enum {
            kTileID_OptionDifficultySlider  =  1,
            kTileID_OptionDifficultyThumb   =  2,
            kTileID_OptionSubtitlesGeneral  =  3,
            kTileID_OptionSubtitlesDialogue =  4,
            kTileID_OptionCrosshair         =  5,
            kTileID_OptionAutosaveOnRest    =  6,
            kTileID_OptionAutosaveOnWait    =  7,
            kTileID_OptionAutosaveOnTravel  =  8,
            kTileID_ButtonResetDefaults     =  9,
            kTileID_ButtonExit              = 10,
         };
         RE::Tile* tileOptDifficultySlider;  // 28 // ID #1
         RE::Tile* tileOptDifficultyThumb;   // 2C // ID #2
         RE::Tile* tileOptSubtitlesGeneral;  // 30 // ID #3
         RE::Tile* tileOptSubtitlesDialogue; // 34 // ID #4
         RE::Tile* tileOptCrosshair;      // 38 // ID #5
         RE::Tile* tileOptAutosaveOnRest; // 3C // ID #6
         RE::Tile* tileOptAutosaveOnWait; // 40 // ID #7
         RE::Tile* tileOptAutosaveOnTravel; // 44 // ID #8
         RE::Tile* tileButtonExit; // 48 // ID#10

         void ResetDefaults();
   };
};