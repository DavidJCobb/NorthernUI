#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/Systems/Timing.h"

namespace RE {
   extern float* const fCachedLoadingBarSegmentPercentages = (float*)0x00B3B3DC; // four floats; cached in LoadingMenu constructor

   DEFINE_SUBROUTINE_EXTERN(void, ForceLoadingScreenProgress,       0x0057B950, UInt32 segmentIndex, float progressWithinSegment);
   DEFINE_SUBROUTINE_EXTERN(void, ForceLoadingScreenProgress_Inner, 0x005ADEC0, UInt32 segmentIndex, float progressWithinSegment); // skips InterfaceManager existence check and other stuff

   class LoadingMenu : public RE::Menu {
      public:
         enum {
            kID   = 0x3EF,
            kVTBL = 0x00A6C40C,
         };
         enum {
            kTileID_OptionDifficultySlider  =  1,
         };

         SInt32 currentSegment = -1; // 28
         UInt32 percentage[4];       // 2C // index == which segment
         SInt32 displayedProgress = -1; // 3C // progress value to write into the UI meter
         float  slideDelay; // 40 // INI:LoadingBar:fLoadingSlideDelay
         UInt32 unk44; // 44
         UInt32 unk48;
         UInt32 unk4C = 0;
         UInt32 unk50 = 0;
         TimeInfo* unk54 = nullptr;
         UInt32 unk58;
         UInt32 unk5C;
         UInt32 unk60 = 0;
         UInt32 unk64 = 0;
         UInt32 unk68; // initialized to unk54::unk10 i.e. current executable time in ms
         UInt32 unk6C; // initialized to unk54::unk10 i.e. current executable time in ms
         UInt8  unk70 = 0;
         UInt8  unk71 = 1;

         //
         // Internally, the loading bar is split into four segments which disproportionately 
         // represent the loading progress. This effectively skews how fast the game seems to 
         // load its content. The portions are, by default,
         //
         // First  fourth of bar =  6% of progress
         // Second fourth of bar = 35% of progress
         // Third  fourth of bar = 44% of progress
         // Last   fourth of bar = 15% of progress
         //
         // The goal here seems to be to prevent two reactions: "ugh, it's barely even started 
         // to load" versus "it's almost done -- come on, hurry up, hurry up!" The amount of 
         // progress used for each bar segment is controlled by INI settings fPercentageOfBar0 
         // through fPercentageOfBar3 under [LoadingBar].
         //
         // Of course, the end user and even the menu XML don't see any of this; within menu 
         // XML, the game just supplies a progress float (normalized based on these portions) 
         // that is then used to fill a single meter, rather than there being four meters 
         // placed end-to-end.
         //

         MEMBER_FN_PREFIX(LoadingMenu);
         DEFINE_MEMBER_FN(Subroutine005AD440, void, 0x005AD440, UInt32);

         // recreation of non-member subroutine at 0x005ADEC0; just call that instead of using this
         void ForceProgress(UInt32 segment, float progress);
   };
};