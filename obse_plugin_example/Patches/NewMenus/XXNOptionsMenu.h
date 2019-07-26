#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "Services/INISettings.h"

#include "Helpers/Checkbox.h"
using namespace XXNMenuHelpers;
#include "Helpers/EnumpickerNonTmpl.h"
using namespace XXNMenuHelpers;

#include <initializer_list>
#include <string>

class XXNOptionsMenu : public RE::Menu {
   public:
      enum { kID = 0x41E };
      XXNOptionsMenu();
      ~XXNOptionsMenu();

      enum {
         kTileID_ButtonExit      = 1,
         kTileID_ListPane        = 2,
         kTileID_ListScrollbar   = 3,
         kTileID_ListScrollThumb = 4,
         kTileID_ButtonRestoreDefaults = 6,
         kTileID_OptionLocalMapRes_Left  = 10,
         kTileID_OptionLocalMapRes_Right = 11,
         kTileID_OptionLocalMapRes_Value = 12,
         kTileID_OptionSuppressDLCSpam   = 13,
         kTileID_OptionUseXXNAlchemyMenu = 14,
         kTileID_OptionShowHUDClock      = 15,
         kTileID_OptionShowHUDInputViewer = 16,
         kTileID_OptionUseXInputIfPatched = 17,
         kTileID_OptionQuantityHandlerDefault_Left  = 18,
         kTileID_OptionQuantityHandlerDefault_Right = 19,
         kTileID_OptionQuantityHandlerDefault_Value = 20,
         kTileID_OptionQuantityHandlerAlt_Left  = 21,
         kTileID_OptionQuantityHandlerAlt_Right = 22,
         kTileID_OptionQuantityHandlerAlt_Value = 23,
         kTileID_OptionQuantityHandlerCtrl_Left  = 24,
         kTileID_OptionQuantityHandlerCtrl_Right = 25,
         kTileID_OptionQuantityHandlerCtrl_Value = 26,
         kTileID_OptionBarterConfirmHandlerDefault_Left  = 27,
         kTileID_OptionBarterConfirmHandlerDefault_Right = 28,
         kTileID_OptionBarterConfirmHandlerDefault_Value = 29,
         kTileID_OptionBarterConfirmHandlerAlt_Left  = 30,
         kTileID_OptionBarterConfirmHandlerAlt_Right = 31,
         kTileID_OptionBarterConfirmHandlerAlt_Value = 32,
         kTileID_OptionBarterConfirmHandlerCtrl_Left  = 33,
         kTileID_OptionBarterConfirmHandlerCtrl_Right = 34,
         kTileID_OptionBarterConfirmHandlerCtrl_Value = 35,
         kTileID_OptionUsePlaystationButtonIcons = 36,
         kTileID_OptionEnhancedMovement = 37,
         kTileID_OptionEnhancedCamera_Left  = 38,
         kTileID_OptionEnhancedCamera_Right = 39,
         kTileID_OptionEnhancedCamera_Value = 40,
         kTileID_OptionCameraInertia_Left  = 41,
         kTileID_OptionCameraInertia_Right = 42,
         kTileID_OptionCameraInertia_Value = 43,
      };
      enum {
         kTraitID_CheckboxState = kTileValue_user20,
         //
         kRootTrait_QuantityHandlerDefault = kTileValue_user10,
         kRootTrait_QuantityHandlerTakeOne = kTileValue_user11,
         kRootTrait_QuantityHandlerTakeAll = kTileValue_user12,
         kRootTrait_BarterConfirmHandlerAlways    = kTileValue_user13,
         kRootTrait_BarterConfirmHandlerIfNotFree = kTileValue_user14,
         kRootTrait_BarterConfirmHandlerNever     = kTileValue_user15,
         kRootTrait_EnhancedCameraBehaviorOblivion = kTileValue_user16,
         kRootTrait_EnhancedCameraBehaviorSkyrim   = kTileValue_user17,
         kRootTrait_EnhancedCameraBehaviorFree     = kTileValue_user18,
         kRootTrait_EnhancedCameraInertiaStandard = kTileValue_user19,
         kRootTrait_EnhancedCameraInertiaFixed    = kTileValue_user20,
         kRootTrait_EnhancedCameraInertiaDisabled = kTileValue_user21,
      };
      enum Confirmation : UInt8 {
         kConfirmationBox_None = 0,
         kConfirmationBox_ResetDefaults,
      };

      RE::Tile* tileButtonExit      = nullptr;
      RE::Tile* tileListPane        = nullptr;
      RE::Tile* tileListScrollbar   = nullptr;
      RE::Tile* tileListScrollThumb = nullptr;
      EnumpickerUInt optionLocalMapRes = EnumpickerUInt(true, { (UInt32)256, (UInt32)512, (UInt32)1024 });
      Checkbox optionSuppressDLCPopups;
      Checkbox optionUseXXNAlchemyMenu = Checkbox(true);
      Checkbox optionShowHUDClock;
      Checkbox optionShowHUDInputViewer;
      Checkbox optionUsePlaystationButtonIcons;
      Checkbox optionUseXInputIfPatched;
      Confirmation confirmation;
      EnumpickerStr optionQuantityHandlerDefault;
      EnumpickerStr optionQuantityHandlerAlt;
      EnumpickerStr optionQuantityHandlerCtrl;
      EnumpickerStr optionBarterConfirmHandlerDefault;
      EnumpickerStr optionBarterConfirmHandlerAlt;
      EnumpickerStr optionBarterConfirmHandlerCtrl;
      Checkbox      optionEnhancedMovement;
      EnumpickerStr optionEnhancedCamera;
      EnumpickerStr optionCameraInertia;

      virtual void	Dispose(bool); // 00 // destructor
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      //virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseUp(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseout(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrame();
      //virtual bool	HandleKeyboardInput(UInt32 inputChar);
      virtual UInt32	GetID();
      //virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1);

      static XXNOptionsMenu* Create(); // create object on the game's own memory heap
      static void Confirm_ResetDefaults();

      bool AnyUnchanged() const;
      void Commit(); // save all changes
      void ResetDefaults();
      void Setup();
};
RE::Tile* ShowXXNOptionsMenu();