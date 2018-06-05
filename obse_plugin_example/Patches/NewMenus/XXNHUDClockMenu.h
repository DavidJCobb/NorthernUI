#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "Miscellaneous/InGameDate.h"

class XXNHUDClockMenu : public RE::Menu {
	public:
		enum { kID = 0x41D };
      enum {
         kMenuModeBoolTrait = kTileValue_user0 // enum name is read by one of our patches
      };
      XXNHUDClockMenu();
		~XXNHUDClockMenu();

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

      static XXNHUDClockMenu* Create(); // create object on the game's own memory heap
      static XXNHUDClockMenu* Get(bool createIfNeeded);

      void Render();
      void Synch();

      Tile* gameTime = nullptr;

      InGameDate date;

      enum {
         kRootTrait_RawGameHour      = kTileValue_user1,
         kRootTrait_FormattedOutput1 = kTileValue_user10,
         kRootTrait_FormattedOutput2 = kTileValue_user11,
         kRootTrait_FormatString1    = kTileValue_user20,
         kRootTrait_FormatString2    = kTileValue_user21,
         //
         kRoot_NumberOfFormattedDates = 2,
      };
};
RE::Tile* ShowXXNHUDClockMenu();
