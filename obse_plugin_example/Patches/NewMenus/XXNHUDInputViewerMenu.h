#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "Miscellaneous/InGameDate.h"

class XXNHUDInputViewerMenu : public RE::Menu {
	public:
		enum { kID = 0x420 };
      static constexpr char* const menuPath = "Data\\Menus\\northernui\\xxnhudinputviewermenu.xml";
      enum {
         kMenuModeBoolTrait = kTileValue_user0 // enum name is read by one of our patches
      };
      enum {
         kRootTrait_InputBitmaskABXY      = kTileValue_user1,
         kRootTrait_InputBitmaskDPad      = kTileValue_user2,
         kRootTrait_InputBitmaskStickTrig = kTileValue_user3,
         kRootTrait_InputBitmaskBumpSys   = kTileValue_user4,
         kRootTrait_InputLeftStickX       = kTileValue_user5,
         kRootTrait_InputLeftStickY       = kTileValue_user6,
         kRootTrait_InputRightStickX      = kTileValue_user7,
         kRootTrait_InputRightStickY      = kTileValue_user8,
         kRootTrait_InputLeftStickMag     = kTileValue_user9,
         kRootTrait_InputRightStickMag    = kTileValue_user10,
         kRootTrait_CurrentGamepadIndex   = kTileValue_user24,
      };
      XXNHUDInputViewerMenu();
		~XXNHUDInputViewerMenu();

      virtual void	Dispose(bool);
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      //virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseUp(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseout(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrame();
      //virtual bool	HandleKeyboardInput(UInt32 inputChar);
      virtual UInt32	GetID();
      //virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1);

      static XXNHUDInputViewerMenu* Create(); // create object on the game's own memory heap
      static XXNHUDInputViewerMenu* Get(bool createIfNeeded);
};
RE::Tile* ShowXXNHUDInputViewerMenu();
