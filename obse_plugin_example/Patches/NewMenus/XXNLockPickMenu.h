#pragma once
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

extern float kLockpickHealth; // TODO: Can we modify the condition of the player's equipped lockpick? Would that be saved?

class XXNLockPickMenu : public RE::Menu {
   public:
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile); // 01
      virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target); // 02
      virtual void	HandleMouseUp  (SInt32 tileID, RE::Tile* target); // 03
      virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target); // 04
      virtual void	HandleMouseout (SInt32 tileID, RE::Tile* target); // 05
      virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target); // 08
      virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target); // 0A
      virtual void	HandleFrame(); // 0B
      virtual bool	HandleKeyboardInput(char inputChar); // 0C
      virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1); // 0E

      struct Lockpick {
         RE::Tile* tile = nullptr; // ID#10 // user0 == animation to play; user1 == x-offset; user2 == y-offset
         float x; // lockpick X-position in normalized pixels
         float y; // lockpick Y-position as a percentage of how far a tumbler can be pressed down
      };
      struct Tumbler {
         RE::Tile* tile = nullptr; // ID #1...5; user0 == solved; user1 == current; user2 == is flubbing
         float cachedMinX = 0;  // X-bound in normalized pixels (absolute coordinate); 0.0 is the left edge of the first tumbler
         float cachedMaxX = 0;  // X-bound in normalized pixels (absolute coordinate)
         float goal;            // how far we need to press the tumbler down; percentage; randomized when we start lockpicking
         float current = 0.0;   // how far down the tumbler has been pressed; percentage
         bool  solved  = false; // whether the tumbler has been solved

         bool  XCoordinateIsOnMe(float coordinate);
         bool  CheckSuccess();
         bool  CheckFailure();
      };

      enum State : UInt8 {
         kState_Idle,
         kState_Pressing,     // lockpick is pressing down
         kState_Flubbing,     // forcing the lockpick up as a tumbler is flubbed
         kState_PickBreaking, // breaking the lockpick as a tumbler is flubbed
         kState_Success,      // lock was successfully picked; waiting for animations to complete
         kState_Closing,      // menu is closing
      };

      RE::Tile* background        = nullptr; // ID #0 // user0 == target tumbler index + 1; user1 == seconds remaining; user2 == seconds max; user3 == pick count; user4 == has skeleton key; user5 == lock level string
      RE::Tile* buttonExit        = nullptr; // ID #6 // user24 == keyboard DXScanCode to trigger this button
      RE::Tile* buttonAutoAttempt = nullptr; // ID #7 // user24 == keyboard DXScanCode to trigger this button

      // ID #8
      // user0  = initial X-position (get; normalized pixels)
      // user1  = current Y-position (set; percentage)
      // user10 = duration of the "break"   blocking-animation
      // user1x = duration of the (x + 1)th blocking-animation
      Lockpick lockpick; // ID #8

      Tumbler tumblers[5];
      //
      float              timeRemaining; // seconds
      float              timeLimit;     // seconds // timer's starting value; varies with lock level; need this so UI has a "current" and "max" value
      RE::TESObjectREFR* lockedRef = nullptr;
      //
      UInt32 lockLevel;
      UInt32 playerSkill; // cap to 0x64
      UInt32 lockpickCount;
      bool   skeletonKey      = false;
      bool   sentAssaultAlarm = false;
      //
      State state            = kState_Idle;
      UInt8 targetTumbler    = 0;
      bool  inputPressDown   = false; // whether the player is telling the menu to press down, irrespective of whether we actually are
      bool  pickIs3D         = false; // cached: whether the lockpick tile is a Tile3D*
      float tumblerFlubTime  = 0.0;   // how long the player is pushing a tumbler through its goal; when this exceeds some value, flub the tumbler

      void UpdateTumblerBounds();
      void OnMenuOpen();
      void ForceClose();
      void SendTrespassAlarm();
      void AutoAttempt();
      void FlubTumbler();
      void BreakLockpick();
      void Failure();
      void Success();
};