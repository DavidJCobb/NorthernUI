#pragma once
#include "obse/NiControllers.h"

namespace RE {
   class BSPlayerDistanceCheckController : public NiTimeController { // sizeof == 0x44
      //
      // If a TESObjectDOOR is an automatic door, then TESObjectDOOR::Unk_45(Arg1) 
      // creates one of these and adds it to an unknown node (likely the door ref 
      // itself).
      //
      // BSPlayerDistanceCheckController::Update seems to remove the controller if 
      // the player steps outside of the auto door activate distance.
      //
      // The callback supplied to the controller apparently fires if the controller 
      // remains attached to the original node long enough for its timer to fire. 
      // The callback checks the player's HighProces::Unk_11F() and if it's not 4, 
      // then the callback writes a TESObjectREFR* to PlayerCharacter::unk5C4. 
      // Research elsewhere suggests that that pointer indicates a reference that 
      // the player is queued to activate automatically.
      //
      public:
         float autoDoorActivateDistance; // 3C // initialized to GMST:fAutoDoorActivateDistance via a setter
         void* autoDoorActivateCallback; // 40
         
         MEMBER_FN_PREFIX(BSPlayerDistanceCheckController);
         DEFINE_MEMBER_FN(SetAutoDoorActivateDistance, void, 0x0060E0C0, float distance);
         DEFINE_MEMBER_FN(SetAutoDoorActivateCallback, void, 0x0060E0D0, decltype(autoDoorActivateCallback) callback);
   };
}
