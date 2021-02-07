#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

class XXNHUDDebugMenu : public RE::Menu {
	public:
		enum { kID = 0x41C };
		XXNHUDDebugMenu();
		~XXNHUDDebugMenu();

      virtual void	Dispose(bool);
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      //virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseUp(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseout(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrame();
      //virtual bool	HandleKeyboardInput(UInt32 inputChar);
      virtual UInt32	GetID();
      //virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1);

      static XXNHUDDebugMenu* Create(); // create object on the game's own memory heap
      static XXNHUDDebugMenu* Get(bool createIfNeeded);

      struct XInput {
         enum {
            kTileID_SmoothWASDStatus   = 1000,
            kTileID_SmoothWASDJoystick = 1001,
            kTileID_EnhancedMovement   = 1002,
         };
         enum {
            kSmoothWASDStatusTrait_IsPressingAncillary = kTileValue_user0,
            kSmoothWASDStatusTrait_DirectionDominant   = kTileValue_user1,
            kSmoothWASDStatusTrait_DirectionAncillary  = kTileValue_user2,
         };
         enum {
            kSmoothWASDJoystickTrait_X          = kTileValue_user0,
            kSmoothWASDJoystickTrait_Y          = kTileValue_user1,
            kSmoothWASDJoystickTrait_Angle      = kTileValue_user2,
            kSmoothWASDJoystickTrait_AngleSlice = kTileValue_user3,
            kSmoothWASDJoystickTrait_IsPerfect8Direction = kTileValue_user4,
         };

         RE::Tile* smoothWASDStatus   = nullptr;
         RE::Tile* smoothWASDJoystick = nullptr;

         bool HandleTileIDChange(SInt32 newID, RE::Tile* tile);

         void ShowSmoothWASDJoystick(SInt32 x, SInt32 y, float angle, SInt8 angleSlice, bool isPerfectDirection);
         void ShowSmoothWASDStatus  (SInt8 dominant, SInt8 ancillary, bool isPressingAncillary);
      } handlerXInput;
      struct EnhancedMovement {
         static constexpr int tile_id = 1002;
         //
         enum class trait_id {
            knocked_down = kTileValue_user0,
            x            = kTileValue_user1,
            y            = kTileValue_user2,
            last_update  = kTileValue_user3,
         };
         //
         RE::Tile* root = nullptr;

         bool HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      } handlerEnhMovement;
};
extern RE::Tile* ShowXXNHUDDebugMenu(); // outside code should call XXNHUDDebugMenu::Get instead
