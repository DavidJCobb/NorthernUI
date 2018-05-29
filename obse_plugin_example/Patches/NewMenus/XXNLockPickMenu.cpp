#include "XXNLockPickMenu.h"

#include "ReverseEngineered/Forms/hardcoded.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "obse/GameObjects.h"

static const UInt32 tumblerIncrements = 3;     // so (Tumbler::goal) can be any multiple of (1 / XXNLockPickMenu::tumblerIncrements)
static const UInt32 timeLimit         = 15.0;  // TODO: vary based on player's skill
static const UInt32 pickSpeedSide     = 5;     // movement speed per ms // TODO: or pull from XML?
static const float  pickSpeedDown     = 0.001; // percentage moved down, per millisecond
static const float  pickSpeedUp       = 0.05;  // percentage moved up,   per millisecond // (percentage * ms) e.g. 50% * 100ms == .5 * .1 == .05
static const float  tumblerSpeedUp    = 0.05;  // percentage moved up,   per millisecond // should be <= pickSpeedUp
static const float  maxFlubTime       = 0.5;   // if the player presses a tumbler too far down for this many seconds, flub the tumbler

static const float  tumblerNotch = (1.0 / tumblerIncrements);

float kLockpickHealth = 1.0F;

void XXNLockPickMenu::OnMenuOpen() {
   srand(time(nullptr)); // seed RNG for our tumbler randomization below
   auto player = (RE::PlayerCharacter*) *g_thePlayer;
   //
   RE::SetInterfaceManagerCursorAlpha(0); // hide the cursor
   //kLockpickHealth     = 1.0;
   this->skeletonKey   = CALL_MEMBER_FN((RE::TESObjectREFR*)player, GetItemCount)(*RE::ptrSkeletonKeyForm);
   this->lockpickCount = CALL_MEMBER_FN((RE::TESObjectREFR*)player, GetItemCount)(*RE::ptrLockpickForm);
   CALL_MEMBER_FN(this->background, UpdateFloat)(kTileValue_user3, this->lockpickCount);
   //
   for (UInt8 i = 0; i < 5; i++) {
      auto& tumbler = this->tumblers[i];
      tumbler.solved  = false;
      tumbler.current = 0.0;
      CALL_MEMBER_FN(tumbler.tile, UpdateFloat)(kTileValue_user1, 0.0);
      tumbler.goal    = ((float) (rand() % tumblerIncrements)) * tumblerNotch; // not good randomness; lower values are less likely
   }
   //
   // TODO:
   //  - get player skill, lock level, etc., etc.
   //
   if (OBLIVION_CAST(this->lockpick.tile, Tile, Tile3D))
      this->pickIs3D = true;
};

void XXNLockPickMenu::SendTrespassAlarm() {
   auto owner = CALL_MEMBER_FN(this->lockedRef, GetOwner)();
   if (owner && this->sentAssaultAlarm == false) {
      auto player = (RE::Actor*) *g_thePlayer;
      SInt32 alarmResult = player->SendTrespassAlarm(this->lockedRef, owner, -1);
      if (alarmResult != -1)
         this->sentAssaultAlarm = true;
   }
};
bool XXNLockPickMenu::Tumbler::XCoordinateIsOnMe(float coordinate) {
   if (coordinate < this->cachedMinX)
      return false;
   if (coordinate > this->cachedMaxX)
      return false;
   return true;
};
bool XXNLockPickMenu::Tumbler::CheckSuccess() {
   float threshold = this->goal + tumblerNotch;
   if (this->current >= threshold)
      return false;
};

void XXNLockPickMenu::HandleTileIDChange(SInt32 tileID, RE::Tile* target) {
   if (tileID == 0) {
      this->background = target;
   } else if (tileID >= 1 && tileID <= 5) {
      this->tumblers[tileID].tile = target;
   } else if (tileID == 6) {
      this->buttonExit = target;
   } else if (tileID == 7) {
      this->buttonAutoAttempt = target;
   } else if (tileID == 8) {
      this->lockpick.tile = target;
   }
};

void XXNLockPickMenu::UpdateTumblerBounds() {
   float zero;
   for (UInt8 i = 0; i < 5; i++) {
      auto& tumbler = this->tumblers[i];
      auto  tile    = tumbler.tile;
      float x = CALL_MEMBER_FN(tile, GetAbsoluteXCoordinate)();
      if (i == 0) {
         zero = x;
         x = 0.0;
      } else
         x -= zero;
      tumbler.cachedMinX = x;
      tumbler.cachedMaxX = x + CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_width);
   }
};
void XXNLockPickMenu::FlubTumbler() {
   this->state = kState_Flubbing;
   this->tumblerFlubTime = 0.0;
   if (!this->skeletonKey) {
      kLockpickHealth -= 0.20;
      if (kLockpickHealth <= 0.0) {
         this->BreakLockpick();
         kLockpickHealth = 1.0;
      }
   }
};
void XXNLockPickMenu::HandleFrame() {
   auto  input      = RE::OSInputGlobals::GetInstance();
   auto  uInterface = RE::InterfaceManager::GetInstance();
   auto& lockpick   = this->lockpick;
   //
   if (uInterface->OtherMenusAreActive(0x3F6))
      RE::SetInterfaceManagerCursorAlpha(0xFF); // don't hide the cursor from other menus
   else
      RE::SetInterfaceManagerCursorAlpha(0);
   //
   bool raising = this->state != kState_Pressing;
   float xPick = lockpick.x;
   if (this->state == kState_Idle) {
      if (!this->inputPressDown) {
         if (lockpick.y == 0) {
            float xMove = RE::InterfaceManager::GetNormalizedMouseAxisMovement(1);
            if (xMove == 0.0) {
               //
               // In the absence of mouse input, test for:
               //  - Gamepad joystick left/right
               //  - Arrow keys left/right
               //  - WASD left/right
               //
               if (CALL_MEMBER_FN(input, QueryKeyboardKeyState)(DIK_RIGHTARROW, RE::kKeyQuery_Hold) || CALL_MEMBER_FN(input, QueryKeyboardKeyState)(DIK_D, RE::kKeyQuery_Hold))
                  xMove += 1.0;
               if (CALL_MEMBER_FN(input, QueryKeyboardKeyState)(DIK_LEFTARROW, RE::kKeyQuery_Hold) || CALL_MEMBER_FN(input, QueryKeyboardKeyState)(DIK_A, RE::kKeyQuery_Hold))
                  xMove -= 1.0;
               //
               // TODO: Multiply xMove by some speed factor
               //
            }
            if (xMove) {
               //
               // Bound the lockpick X-coordinate to the tumbler area, and update the XML.
               //
               float xMax = this->tumblers[4].cachedMaxX;
               xPick += xMove;
               if (xPick < 0.0)
                  xPick = 0.0;
               if (xPick > xMax)
                  xPick = xMax;
               CALL_MEMBER_FN(lockpick.tile, UpdateFloat)(kTileValue_user1, xPick);
               lockpick.x = xPick;
            }
            //
            // xPick will be used below.
         } else {
            //
            // Move the lockpick back up.
            //
            float y = max(0.0, lockpick.y - pickSpeedUp);
            lockpick.y = y;
            CALL_MEMBER_FN(lockpick.tile, UpdateFloat)(kTileValue_user1, y);
         }
      } else { // !!this->inputPressDown
         if (this->targetTumbler < 5)
            this->state = kState_Pressing;
      }
   } else if (this->state == kState_Pressing) {
      if (this->inputPressDown) {
         float y = min(1.0, lockpick.y + pickSpeedDown);
         if (this->targetTumbler < 5) {
            //
            // The player should be targeting a tumbler. Press it down, too.
            //
            auto& tumbler = this->tumblers[this->targetTumbler];
            if (y > tumbler.goal) {
               y = tumbler.goal;
               //
               // The player has started to flub a tumbler... if they keep pressing 
               // it down for too long, flub it.
               //
               CALL_MEMBER_FN(tumbler.tile, UpdateFloat)(kTileValue_user2, 2.0);
               float time = this->tumblerFlubTime + 0.001;
               if (time > maxFlubTime) {
                  time = 0.0;
                  this->FlubTumbler();
               }
               this->tumblerFlubTime = time;
            }
            tumbler.current = y;
         }
         lockpick.y = y;
         CALL_MEMBER_FN(lockpick.tile, UpdateFloat)(kTileValue_user1, y);
      } else { // !this->inputPressDown
         this->state = kState_Idle;
      }
   } else if (this->state == kState_Flubbing) {
      //
      // Pop the lockpick and flubbed tumbler back up.
      //
      float y = lockpick.y - pickSpeedUp;
      if (y <= 0.0) {
         this->state = kState_Idle;
         y = 0.0;
      }
      if (this->targetTumbler < 5) {
         auto& tumbler = this->tumblers[this->targetTumbler];
         tumbler.current = y;
      }
      lockpick.y = y;
   } else if (this->state == kState_PickBreaking) {
      auto nif = (RE::Tile3D*) lockpick.tile;
      if (!this->pickIs3D || !nif->IsAnimating()) { // animation impossible or animation complete
         lockpick.y = 0.0;
         this->state = kState_Idle;
      }
   } else if (this->state == kState_Success) {
      return;
   } else if (this->state == kState_Closing) {
      return;
   }
   //
   // Loop over tumblers:
   //  - Update index of tumbler we're currently over, if any
   //  - Check whether all tumblers are solved
   //  - Animate tumblers that have been released back to equilibrium
   //
   this->UpdateTumblerBounds();
   this->targetTumbler = -1;
   bool solved = true;
   for (UInt8 i = 0; i < 5; i++) {
      bool  raising = true;
      auto& tumbler = this->tumblers[i];
      if (!tumbler.solved)
         solved = false;
      if (tumbler.XCoordinateIsOnMe(xPick)) {
         this->targetTumbler = i;
         if (this->state == kState_Pressing)
            raising = false;
      }
      if (raising) {
         float c = tumbler.current;
         float bound = 0.0;
         if (tumbler.solved)
            //
            // Animate the tumbler back to its goal, if the player pressed it down only slightly beneath
            //
            bound = tumbler.goal;
         c = max(bound, c - tumblerSpeedUp);
         if (tumbler.current != c) {
            tumbler.current = c;
            CALL_MEMBER_FN(tumbler.tile, UpdateFloat)(kTileValue_user1, c);
         }
      }
   }
   //
   // Pass remaining state to XML:
   //
   {
      auto tile = this->background;
      CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user0, this->targetTumbler);
      CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user1, this->timeRemaining);
      CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user2, timeLimit);
      //
      // ...
      //
   }
   if (solved) {
      this->Success();
   } else {
      //
      // Update time limit
      //
      float s = max(0.0, this->timeRemaining - 0.001);
      this->timeRemaining = s;
      if (s == 0.0) {
         Failure();
      }
   }
   {  // Handle keyboard shortcuts
      UInt32 key;
      //
      // TODO: Instead of calling the functions directly, route through this->HandleClick(int, Tile*);
      //
      {  // Auto-attempt
         key = (SInt32) CALL_MEMBER_FN(this->buttonAutoAttempt, GetFloatTraitValue)(kTileValue_user24);
         if (key > 0 && key < 256) {
            if (CALL_MEMBER_FN(input, QueryKeyboardKeyState)(key, RE::kKeyQuery_Down))
               this->AutoAttempt();
         }
      }
      {  // Close
         key = (SInt32) CALL_MEMBER_FN(this->buttonExit, GetFloatTraitValue)(kTileValue_user24);
         if (key > 0 && key < 256) {
            if (CALL_MEMBER_FN(input, QueryKeyboardKeyState)(key, RE::kKeyQuery_Down))
               this->ForceClose();
         }
      }
   }
};
void XXNLockPickMenu::HandleMouseDown(SInt32 tileID, RE::Tile* target) {
   if (target == this->buttonExit || target == this->buttonAutoAttempt)
      return;
   this->inputPressDown = true;
};
void XXNLockPickMenu::HandleMouseUp(SInt32 tileID, RE::Tile* target) {
   this->inputPressDown = false;
   if (this->state != kState_Closing && this->state != kState_Success) {
      if (target == this->buttonExit) {
         //
         // TODO
         //
         return;
      }
      if (target == this->buttonAutoAttempt) {
         //
         // TODO
         //
         return;
      }
      if (this->state == kState_Idle) {
         if (this->targetTumbler < 5) {
            auto& tumbler = this->tumblers[this->targetTumbler];
            if (tumbler.current >= tumbler.goal) {
               tumbler.solved = true;
               CALL_MEMBER_FN(tumbler.tile, UpdateFloat)(kTileValue_user0, 2.0);
            }
         }
      }
   }
};
bool XXNLockPickMenu::HandleNavigationInput(NavigationInput argEnum, float arg1) {
   //
   // catch left/right arrow keys if shift is not held
   //
   if (argEnum == kNavInput_Left || argEnum == kNavInput_Right) // if shift is not held, catch left/right arrow keys for our use
      return true;
   //if (argEnum == kNavInput_Down) { // catch down arrow key for our use
   //   return true;
   //}
   return false;
};

void XXNLockPickMenu::AutoAttempt() {
   //
   // TODO
   //
};
void XXNLockPickMenu::BreakLockpick() {
   //
   // TODO:
   //  - If the player isn't using the Skeleton Key:
   //     - Decrement local lockpick count
   //     - Remove lockpick from the player's inventory
   //     - Play lock-breaking animation: this->PlayAnimation(kAnimation_Break)
   //        - Animation-complete handler will take it from here
   //     - Return
   //
   auto player = (RE::PlayerCharacter*) *g_thePlayer;
   auto skelly = CALL_MEMBER_FN((RE::TESObjectREFR*)player, GetItemCount)(*RE::ptrSkeletonKeyForm);
   if (!skelly) {
      this->state = kState_PickBreaking;
      //
      float picks = this->lockpickCount - 1;
      //
      CALL_MEMBER_FN(this, PlaySoundByEditorID)("UILockPickBreak");
      CALL_MEMBER_FN(this, PlaySoundByEditorID)("UILockTumblerFall"); // ?
      //
      player->RemoveItem(*RE::ptrLockpickForm, 0, 1, 0, 0, 0, 0, 0, 1, 1);
      this->lockpickCount = picks;
      CALL_MEMBER_FN(this->background, UpdateFloat)(kTileValue_user3, picks);
      player->miscStats[0x24]++;
      //
      if (picks <= 0.0) {
         CALL_MEMBER_FN(this, PlaySoundByEditorID)("DRSLockOpenFail");
         RE::SetInterfaceManagerCursorAlpha(0xFF); // stop hiding the cursor
         // TES4_005AF960(); // specific to LockPickMenu; decode it; probably closes the menu
         ShowMessageBox("You have run out of lockpicks.", 0, 1, "OK", 0);
         return;
      }
      //
      // TODO: play lockpick-break animation; our code is dependent on this
      //
   }
};
void XXNLockPickMenu::ForceClose() {
   // Per 0x005AF960:
   RE::SetInterfaceManagerCursorAlpha(0xFF);
   if (this->state != kState_Closing) {
      //
      // - return if any conditions (e.g. pending animations) are not complete
      //
      // - tear down any persistent sounds (e.g. as LockPickMenu does for its tumblers; see loop at 005AF9CC)
      //
      this->Close();
      this->state = kState_Closing;
   }
};
void XXNLockPickMenu::Failure() {
   //
   // TODO:
   //  - Punt the user out of the menu
   //
};
void XXNLockPickMenu::Success() {
   auto player = (RE::PlayerCharacter*) *g_thePlayer;
   //
   // TODO:
   //  - Unlock the reference
   //  - Give the player their lockpicking experience
   //     - Do it five times, to compensate for the fact that we're not doing it per-tumbler
   //     - (*g_thePlayer)->ModExperience(kActorVal_Security, 0, 0.0);
   //  - Examine HandlePossibleSuccess in OllyDbg; there are player- and AI-related functions we still need to decode
   //  - Punt the user out of the menu
   //
   CALL_MEMBER_FN(this->lockedRef, Unlock)();
   CALL_MEMBER_FN(this, PlaySoundByEditorID)("UILockSuccess");
   CALL_MEMBER_FN(this, PlaySoundByEditorID)("DRSLockOpen");
   RE::SetInterfaceManagerCursorAlpha(0xFF); // stop hiding the cursor
   player->miscStats[20]++; // locks picked
   // TES4_005AF960(); // specific to LockPickMenu; decode it; probably closes the menu
   if (this->sentAssaultAlarm) {
      //
      // DECODE THIS AND IMPLEMENT IT:
      //
      // auto eax = *(0x00B3BD00).TES4_00675BF0(*g_thePlayer, this->lockedRef, 2);
      // if (eax) {
      //    edi = *(0x00B3BD00).TES4_00675740(eax, 0);
      //    if (edi) {
      //       do {
      //          auto ecx = edi->unk00;
      //          float a = ecx->TES4_004D7E90(*g_thePlayer, 0);
      //          if (a <= 400.0)
      //             return;
      //          auto eax = edi->unk04;
      //          if (!eax) {
      //             edi->unk00 = 0;
      //             continue;
      //          }
      //          edi->unk04 = eax->unk04;
      //          edi->unk00 = eax->unk00;
      //          FormHeap_Free(eax);
      //       } while (true);
      //       edi->~Destructor();
      //       FormHeap_Free(edi);
      //    }
      // }
   }
   player->isMovingIntoNewSpace = 1; // vanilla does this; don't know why
   CALL_MEMBER_FN(this->lockedRef, Activate)((RE::TESObjectREFR*) *g_thePlayer, 0, 0, 1);
   player->isMovingIntoNewSpace = 0; // vanilla does this; don't know why
};