#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class AudioMenu : public RE::Menu {
      public:
         enum {
            kID   = 0x3F9,
            kVTBL = 0x00A6B29C,
         };
         enum {
            kTileID_VolumeMasterSlider   = 1,
            kTileID_VolumeMasterThumb    = 2,
            kTileID_VolumeVoiceSlider    = 3,
            kTileID_VolumeVoiceThumb     = 4,
            kTileID_VolumeEffectsSlider  = 5,
            kTileID_VolumeEffectsThumb   = 6,
            kTileID_VolumeFootstepSlider = 7,
            kTileID_VolumeFootstepThumb  = 8,
            kTileID_VolumeMusicSlider    = 9,
            kTileID_VolumeMusicThumb    = 10,
            kTileID_ButtonResetDefaults = 11,
            kTileID_ButtonExit          = 12,
         };
         RE::Tile* tileVolumeMasterSlider;   // 28 // ID #1
         RE::Tile* tileVolumeMasterThumb;    // 2C // ID #2
         RE::Tile* tileVolumeVoiceSlider;    // 30 // ID #3
         RE::Tile* tileVolumeVoiceThumb;     // 34 // ID #4
         RE::Tile* tileVolumeEffectsSlider;  // 38 // ID #5
         RE::Tile* tileVolumeEffectsThumb;   // 3C // ID #6
         RE::Tile* tileVolumeFootstepSlider; // 40 // ID #7
         RE::Tile* tileVolumeFootstepThumb;  // 44 // ID #8
         RE::Tile* tileVolumeMusicSlider;    // 48 // ID #9
         RE::Tile* tileVolumeMusicThumb;     // 4C // ID#10
         RE::Tile* tileButtonExit;           // 50 // ID#12

         void ResetDefaults();
   };
};