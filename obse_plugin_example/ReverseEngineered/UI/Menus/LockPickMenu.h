#pragma once
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameMenus.h"
#include "obse/GameObjects.h"
#include "obse/NiTypes.h"
#include "obse/Utilities.h"

namespace RE {
   class LockPickMenu : public Menu { // sizeof == 0x180
      public:
         LockPickMenu();
         ~LockPickMenu();
         enum {
            kID   = 0x3F6,
            kVTBL = 0x00A6C73C,
         };

         enum LockState : UInt32 {
            kLockState_Idle      = 0, // initial state; also set by a subroutine that animates the lockpick to idle if possible
            kLockState_Success   = 1, // only set by void LockPickMenu::HandlePossibleSuccess
            kLockState_Unused2   = 2, // never set by LockPickMenu code?
            kLockState_Breaking  = 3, // only set by void LockPickMenu::BreakLockpick()
            kLockState_Resetting = 4, // only set by void LockPickMenu::ResetSomeTumblersOnPickBreakage()
            kLockState_Unused5   = 5, // never set by LockPickMenu code?
            kLockState_Closing   = 6, // see non-member subroutine 0x005AF960
         };

         struct Tumbler { // sizeof == 0x28
            //
            // See notes on LockPickMenu::tumblers (0x7C) below.
            //
            float  heightOffset; // 00 // how far the tumbler has fallen; XML is expected to displace the tile's Y-coordinate downward by this value
            UInt32 hangStart; // 04 // time in milliseconds that the tumbler reached the top
               // - starts off at UINT_MAX when the menu is opened
               // - set to UINT_MAX when the tumbler is reset as the result of the lockpick breaking on a different tumbler
               // - set to current executable time in milliseconds whenever we play sound UILockClickNow
               // - doesn't get cleared when a tumbler is successfully locked into place, so it also indicates when that happened
            UInt32 hangTime; // 08 // time in milliseconds that the tumbler stays up before starting to fall; not initialized until the tumbler first moves
            float  unk0C; // initial rise speed? if the lockpick is moving up under a tumbler and the tumbler's velocity is zero, this is the new velocity
            float  unk10; // related to how fast the tumbler falls
            float  velocity; // 14 // how fast the tumbler moves; up is positive, down is negative
               // - if the tumbler is reset as the result of a lockpick breaking, this is set to -LockPickMenu::unk6C
            bool   isMoving = false; // 18
               // - set to 0 when the tumbler is solved
               // - set to 1 when the tumbler is reset as the result of the lockpick breaking on a different tumbler
               // - indicates that the tumbler is in motion?
               //    - we can verify that. when you've successfully solved a tumbler, code 
               //      requires that this be set to false before you get Security EXP. We 
               //      can solve a tumbler and query the Security stat while the tumbler 
               //      is still moving (unless the window for that is too small...).
            bool   isSolved = false; // 19
            UInt8  unk1A = 0;
               // - set to 1 when the tumbler is reset as the result of the lockpick breaking on a different tumbler
            UInt8  unk1B; // not used? haven't seen it yet
            SInt32 unk1C; // center of the tumbler; read from the XML when the menu is first opened
            Tile*  tile = nullptr; // 20 // ID#11, #12, #13, #14, #15 == nif#lockpick_tumbler_1, nif#lockpick_tumbler_2, etc.
            void*  unk24; // stores the result of a call to play UILockTumblerMoveLP, apparently
               // - type is OSSoundGlobals::Sound*?
         };

         Tile*  tileBackground = nullptr; // 28 // ID #1 // image#lockpick_background
            //
            // Its X- and Y-coordinates are used in calculations related to tumbler positions, and 
            // these positions are important to how the overall lockpick behavior works. However, 
            // I haven't figured out the exact behavior yet.
            //
            // user2 == lock level string
            // user3 == picks remaining
            // user4 == player Security level
            //
         Tile*  buttonExit        = nullptr; // 2C // ID #2 // string gets set by the executable
         Tile*  unk30             = nullptr; // 30 // ID #4 // rect#glass_window
         Tile*  buttonAutoAttempt = nullptr; // 34 // ID #5 // string gets set by the executable
         TESObjectREFR* lockedRef = nullptr; // 38 // locked ref being picked
         UInt32 lockpickCount = 0; // 3C // includes the Skeleton Key; set when the menu is opened
         UInt32 unk40; // 40 // skill? gets capped to 0x64 (100) in some places
         UInt32 unk44; // 44 // related to unk40
         UInt32 lockLevel = 0; // 48 // set when the menu is opened
         UInt32 solvedTumblerCount = 0; // 4C
            //
            // Set to the following values for the following lock levels:
            // 1: Very Easy
            // 2: Easy
            // 3: Average
            // 4: Hard
            // 5: All Others
            //
         UInt32 unk50; // set to   5 when the menu is first opened
         UInt32 unk54; // set to 300 when the menu is first opened
         float  unk58; // set to 0.9 when the menu is first opened
         float  unk5C;
         float  unk60 = 8.0; // set to 0.020 when the menu is first opened
         float  unk64 = 4.0; // set to 0.003 when the menu is first opened
         float  unk68; // set to 0.005000 when the menu is first opened
         float  unk6C; // set to 0.020000 when the menu is first opened
         UInt32 unk70; // set to 0.000001 when the menu is first opened
         float  unk74; // set to 15.0 when the menu is first opened
         float  unk78; // maximum value for tumbler heightOffsets
            //
            // Default value is (2 / 3), set in ShowLockPickMenu. If any tumbler has a Tile3D and a 
            // valid NiControllerSequence, then this is the Stop Time for that sequence. Based on 
            // the vanilla XML, this would be the Stop Time for the "UP" sequence in Tumbler02.nif, 
            // which is (2 / 3).
            //
         Tumbler tumblers[5]; // 7C
            // 
            // It was hard to tell where the Tumbler instances begin and end, because the constructor 
            // and all accessors do that stupid bullcrap where the pointer is set to the middle of 
            // the struct, and fields in the first half or so are accessed with negative offsets. 
            // It's never even the SAME PLACE in the middle with the various accessors!
            //
            // Here are the raw fields, so you know how I pieced it together.
            // 
            // = 1 =====================================================
            // float  unk7C;
            // SInt32 unk80; // repeats every 0x28 bytes...
            //        ...
            // float  unk88;
            // float  unk8C;
            // float  unk90;
            // UInt8  unk94;
            // UInt8  unk95;
            // UInt8  unk96;
            // UInt8  pad97; // not sure if unused
            // SInt32 unk98  // center of tumbler 1, per ShowLockPickMenu
            // Tile*  unk9C  // ID#11 // nif#lockpick_tumbler_1
            // void*  unkA0;
            // = 2 =====================================================
            // float  unkA4; // unk7C for Tumbler 2
            // SInt32 unkA8; // unk80 for Tumbler 2
            //        ...
            // Tile*  unkC4  // ID#12 // nif#lockpick_tumbler_2
            //        ...
            // = 3 =====================================================
            //        ...
            // Tile*  unkEC  // ID#13 // nif#lockpick_tumbler_3
            //        ...
            // = 4 =====================================================
            //        ...
            // Tile*  unk114 // ID#14 // nif#lockpick_tumbler_4
            //        ...
            // = 5 =====================================================
            //        ...
            // Tile*  unk13C // ID#15 // nif#lockpick_tumbler_5
            //        ...
         // //
         Tile*  tileBolt; // 0144 // ID#20 // nif#lockpick_bolt // game dynamic-casts to Tile3D*
         float  lockpickX; // 0148 // gets set to the center of tumbler 1 when the menu is first opened // possibly lockpick x-position
         float  lockpickY; // 014C // related to the Y-offset of the lockpick NIF; pick NIF user3 gets set to dec(290) - unk14C
         LockState state; // 150
         float  unk154 = 5.0; // gets set to 0.34 when the menu is first opened // same unit/type/etc. as unk158
         float  unk158; // Y-axis offset, length, coordinate, etc., for something
         float  unk15C; // gets set to 0.0014 when the menu is first opened
         SInt32 unk160 = 0; // tumbler index; negative means not a valid tumbler; otherwise not bounds-checked // tumbler the player is interacting with?
         UInt32 unk164; // unk160 is written here when the menu is first opened
         SInt32 unk168 = -1; // tumbler index; tumbler that the pick is being moved toward? forced to -1 when a tumbler is in the air, which may prevent movement?
         UInt32 unk16C;
         UInt32 unk170;
         Tile*  unk174 = nullptr; // apparently set to the tile in a Tumbler
         Tile3D* tileLockpick; // 0178 // ID #3 // nif#lockpick_pick // filename is set by the executable based on whether the player has a Skeleton Key
         bool   sentTrespassAlarm = false; // 17C
            // - gets set to 1 when the player starts lockpicking (Menu::Unk_02), if a SendTrespassAlarm on the lock owner returns a result != -1
            // - of course, it then gets set to 1 immediately after, either way... inlined code, maybe?
            // - I think there's an inlined method to send a trespass alarm on the locked ref, and this bool tracks whether we've done it yet
         UInt8  pad17D;
         UInt8  pad17E;
         UInt8  pad17F;

         MEMBER_FN_PREFIX(LockPickMenu);
         DEFINE_MEMBER_FN(GetTumblerClosestTo,             UInt8, 0x005AF190, SInt32 xPosition); // returns tumbler index; argument is same measurement as unk148
         DEFINE_MEMBER_FN(PlaySoundByEditorID,             void,  0x005AFD50, const char* editorID);
         DEFINE_MEMBER_FN(ResetSomeTumblersOnPickBreakage, void,  0x005AF200); // this->unk160 identifies the tumbler that the pick broke on
         DEFINE_MEMBER_FN(UpdateTumblerHeightOffsets,      void,  0x005B0830); // applies gravity, springiness, etc., to all tumblers

         // TODO: Member 005AFA80 does something involving input
         // TODO: Look into non-member 005AF960

         void SendTrespassAlarm() { // gets inlined in Bethesda's code
            auto owner = CALL_MEMBER_FN(this->lockedRef, GetOwner)();
            if (owner && !this->sentTrespassAlarm) {
               auto player = (RE::Actor*) *g_thePlayer;
               SInt32 alarmResult = player->SendTrespassAlarm(this->lockedRef, owner, -1);
               if (alarmResult != -1)
                  this->sentTrespassAlarm = true;
            }
         };
   };
   static_assert(sizeof(LockPickMenu::Tumbler) >= 0x28, "RE::LockPickMenu::Tumbler is too small!");
   static_assert(sizeof(LockPickMenu::Tumbler) <= 0x28, "RE::LockPickMenu::Tumbler is too large!");
   static_assert(sizeof(LockPickMenu) >= 0x180, "RE::LockPickMenu is too small!");
   static_assert(sizeof(LockPickMenu) <= 0x180, "RE::LockPickMenu is too large!");
   static_assert(offsetof(LockPickMenu, unk154) == 0x154, "RE::LockPickMenu::unk154 is positioned incorrectly!");

   extern const char*** const lockLevelNames; // *lockLevelNames[a][b] == const char* ?

   // Opcodes at 0x005AF90A (ShowLockPickMenu + 0x4CA) vary the lockpick NIF depending on whether your pick is normal or the Skeleton Key

   // const auto unkBool1 = (UInt8*)0x00B3B3F4; // if true, then sound DRSLockOpenFail needs to play?
   // const auto unkBool2 = (UInt8*)0x00B3B3F5;
   // const auto unkBool3 = (UInt8*)0x00B3B3F6;

   // Subroutine 0057B6A0 is void StartLockpickingOnReference(TESObjectREFR* target)
   // Note that its caller(s?) handle(s?) the "requires key" behavior
};