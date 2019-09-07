#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   DEFINE_SUBROUTINE_EXTERN(void, CloseControlsMenu, 0x0059B9E0);

   class ControlsMenu : public Menu { // sizeof == 0xE8
      public:
         enum { kVTBL = 0x00A69FF4 };
         enum {
            kTileID_ButtonExit = 1,
            kTileID_MappingsScrollbar   = 2,
            kTileID_MappingsScrollThumb = 3,
            kTileID_MappingsListPane    = 4,
            kTileID_LookSensitivitySlider = 5,
            kTileID_LookSensitivityThumb  = 6,
            kTileID_InvertY    = 7,
            kTileID_SwapSticks = 8,
            kTileID_RemapControlsButton   =  9, // opens a submenu
            kTileID_ButtonResetToDefaults = 10,
            kTileID_RemapInstructionsText = 11,
            kTileID_DeviceEnumpickerLeft  = 12,
            kTileID_DeviceEnumpickerRight = 13,
            //
            // IDs >= 0xD are remappable controls
            //
         };
         enum Device : UInt32 {
            kDevice_Keyboard = 0,
            kDevice_Mouse    = 1,
            kDevice_Joystick = 2,
            kDevice_Undefined = 0xFF,
         };
         enum MenuPage {
            kMenuPage_Base      = 1,
            kMenuPage_Remapping = 2,
         };

         union { // bethesda... why are you like this?
            Tile* tiles[0xD]; // 28 // initialized with a memset to zero
            struct {
               Tile* tileButtonExit; // 28
               Tile* tileMappingsScrollbar; // 2C
               Tile* tileMappingsScrollThumb; // 30
               Tile* tileMappingsListPane; // 34
               Tile* tileLookSensSlider;
               Tile* tileLookSensThumb;
               Tile* tileButtonInvertY;
               Tile* tileButtonSwapSticks;
               Tile* tileOpenRemapList;
               Tile* tileButtonResetToDefaults;
               Tile* tileRemapInstructionsText;
               Tile* tileDeviceEnumpickerLeft;
               Tile* tileDeviceEnumpickerRight;
            };
         };
         Device currentDevice = kDevice_Undefined; // 5C
         UInt8  unk60[0x74]; // memset'd to zero on construct
         bool   queuedRedrawControls = false; // D4 // if true, we redraw the control options and force (controlToRemap) to nullptr
         UInt8  padD5[3];
         Tile*  controlToRemap = nullptr; // D8 // HandleMouseUp does nothing if this is non-zero // same type as argument to StartRemapping
         float  scrollbarStepSize; // DC // when remapping, we set the scrollbar step and jump sizes to zero so you can't scroll; we need to restore them after remapping
         float  scrollbarJumpSize; // E0
         UInt8  unkE4 = 0;
         UInt8  padE5[3];

         MEMBER_FN_PREFIX(ControlsMenu);
         DEFINE_MEMBER_FN(TileIDToTileArrayIndex, UInt32, 0x00587500, SInt32 tileID); // returns 0xD if not a valid tile ID
         DEFINE_MEMBER_FN(SelectPreviousDevice,   void,   0x0059B980);
         DEFINE_MEMBER_FN(SelectNextDevice,       void,   0x0059B920);
         DEFINE_MEMBER_FN(StartRemapping,         void,   0x0059C3F0, Tile* controlToRemap);

         SInt32 getMenuPage();
         bool isInMappings();
         bool isRemapping();

         void backOutOfMapping();
   };
   //
   // MSVC chokes on this but you can use Intellisense in VS2015 to check it; just 
   // uncomment it and then comment it back out before compiling. Not ideal but 
   // you do what you can
   //
   //static_assert(offsetof(ControlsMenu, tiles[1]) == offsetof(ControlsMenu, tileMappingsScrollbar), "Your compiler/IDE isn't aligning the members properly.");
};