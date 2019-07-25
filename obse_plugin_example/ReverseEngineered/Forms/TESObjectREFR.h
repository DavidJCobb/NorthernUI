#pragma once
#include "shared.h"
#include "obse/GameExtraData.h"
#include "obse/GameObjects.h"

class TESContainer;
namespace RE {
   class TESObjectREFR : public ::TESObjectREFR {
      public:
         enum REFRFlags {
            kREFRFlag_CollisionDisabled = 0x00000010,
         };
         enum KnockedState : UInt8 {
            kKnockedState_Normal      = 0,
            kKnockedState_KnockedDown = 1,
            kKnockedState_KnockedOut  = 2, // unconscious essential, paralyzed, negative stamina, etc.
         };
         enum OpenState : UInt8 {
            kOpenState_None = 0,
            kOpenState_Open = 1,
            kOpenState_Opening = 2,
            kOpenState_Closed  = 3,
            kOpenState_Closing = 4,
         };
         enum SitSleepState : UInt8 { // guesses
            kSitSleepState_NotSitting = 0,
            kSitSleepState_LoadingSitIdle = 1,
            kSitSleepState_ReadyToSit = 2,
            kSitSleepState_Sitting = 3,
            kSitSleepState_ReadyToStand = 4,
            kSitSleepState_UnknownSit05 = 5,
            kSitSleepState_LoadingSleepIdle = 6,
            kSitSleepState_ReadyToSleep = 7,
            kSitSleepState_ReadyToSleep_B = 8,
            kSitSleepState_Sleeping = 9,
            kSitSleepState_ReadyToWakeUp = 10,
         };

         MEMBER_FN_PREFIX(TESObjectREFR);
         DEFINE_MEMBER_FN(Activate,                void,                  0x004DD260, TESObjectREFR* activatedBy, UInt32, UInt32, UInt32);
         DEFINE_MEMBER_FN(ActivatingWouldBeACrime, bool,                  0x004DEBF0);
         DEFINE_MEMBER_FN(GetBaseTESContainer,     TESContainer*,         0x004D6D40);
         DEFINE_MEMBER_FN(GetCurrentWorldspace,    TESWorldSpace*,        0x004D6670);
         DEFINE_MEMBER_FN(GetExtraMapMarkerData,   ExtraMapMarker::Data*, 0x004D7730);
         DEFINE_MEMBER_FN(GetFaceAnimationData,    NiObject*,             0x004D66B0, NiNode* refrRootNode);
         DEFINE_MEMBER_FN(GetFullName,             const char*,           0x004DA2A0);
         DEFINE_MEMBER_FN(GetHUDReticleIconEnum,   UInt32,                0x004DE980);
         DEFINE_MEMBER_FN(GetInventoryItemByIndex, ExtraContainerChanges::EntryData*, 0x004D88F0, UInt32 index, bool useDataHandler); // for looping, and meant for use by menus; returns a NEW COPY of the EntryData in the ref's inventory; caller is responsible for deleting the copy // specify false for the bool
         DEFINE_MEMBER_FN(GetInventoryItemCount,   UInt32,                0x004D8950, UInt32); // gets the max index + 1 for GetInventoryItemByIndex calls
         DEFINE_MEMBER_FN(GetItemCount,            UInt32,                0x005E0F00, TESForm* item);
         DEFINE_MEMBER_FN(GetOpenState,            OpenState,             0x004DE660);
         DEFINE_MEMBER_FN(GetOwner,                TESForm*,              0x004DB6B0);
         DEFINE_MEMBER_FN(GetPitch,                float,                 0x004A9720); // literally just returns rotX
         DEFINE_MEMBER_FN(IsCreature,              bool,                  0x005E3270);
         DEFINE_MEMBER_FN(IsHorse,                 bool,                  0x004D74D0);
         DEFINE_MEMBER_FN(IsInInterior,            bool,                  0x004D8B90);
         DEFINE_MEMBER_FN(SetPitch,                void,                  0x004D89D0, float);
         DEFINE_MEMBER_FN(Unlock,                  void,                  0x004DBEA0);

         bool IsEmpty();
         bool IsEmptyForUIPurposes();
   };

   DEFINE_SUBROUTINE_EXTERN(ExtraContainerChanges::Data*, GetContainerChangeData, 0x00485E00, TESObjectREFR*, TESContainer*);
};