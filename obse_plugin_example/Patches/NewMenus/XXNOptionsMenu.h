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
      };
      enum {
         kTraitID_CheckboxState = kTileValue_user20,
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
      Checkbox optionSuppressDLCPopups; // doesn't require restart because this menu manually tells the patch to reapply
      Checkbox optionUseXXNAlchemyMenu = Checkbox(true);
      Checkbox optionShowHUDClock; // this menu manually tells the patch to update
      Confirmation confirmation;

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