#pragma once
#include "shared.h"
#include "obse/GameExtraData.h"
#include "obse/GameObjects.h"

class TESContainer;
namespace RE {
   class TESObjectREFR : public ::TESObjectREFR {
      public:
         enum OpenState : UInt8 {
            kOpenState_None = 0,
            kOpenState_Open = 1,
            kOpenState_Opening = 2,
            kOpenState_Closed  = 3,
            kOpenState_Closing = 4,
         };

         MEMBER_FN_PREFIX(TESObjectREFR);
         DEFINE_MEMBER_FN(Activate,                void,                  0x004DD260, TESObjectREFR* activatedBy, UInt32, UInt32, UInt32);
         DEFINE_MEMBER_FN(ActivatingWouldBeACrime, bool,                  0x004DEBF0);
         DEFINE_MEMBER_FN(GetBaseTESContainer,     TESContainer*,         0x004D6D40);
         DEFINE_MEMBER_FN(GetCurrentWorldspace,    TESWorldSpace*,        0x004D6670);
         DEFINE_MEMBER_FN(GetExtraMapMarkerData,   ExtraMapMarker::Data*, 0x004D7730);
         DEFINE_MEMBER_FN(GetFullName,             const char*,           0x004DA2A0);
         DEFINE_MEMBER_FN(GetHUDReticleIconEnum,   UInt32,                0x004DE980);
         DEFINE_MEMBER_FN(GetInventoryItemByIndex, ExtraContainerChanges::EntryData*, 0x004D88F0, UInt32 index, bool useDataHandler); // for looping, and meant for use by menus; returns a NEW COPY of the EntryData in the ref's inventory; caller is responsible for deleting the copy // specify false for the bool
         DEFINE_MEMBER_FN(GetInventoryItemCount,   UInt32,                0x004D8950, UInt32); // gets the max index + 1 for GetInventoryItemByIndex calls
         DEFINE_MEMBER_FN(GetItemCount,            UInt32,                0x005E0F00, TESForm* item);
         DEFINE_MEMBER_FN(GetOpenState,            OpenState,             0x004DE660);
         DEFINE_MEMBER_FN(GetOwner,                TESForm*,              0x004DB6B0);
         DEFINE_MEMBER_FN(IsHorse,                 bool,                  0x004D74D0);
         DEFINE_MEMBER_FN(IsInInterior,            bool,                  0x004D8B90);
         DEFINE_MEMBER_FN(Unlock,                  void,                  0x004DBEA0);

         bool IsEmpty();
         bool IsEmptyForUIPurposes();
   };

   DEFINE_SUBROUTINE_EXTERN(ExtraContainerChanges::Data*, GetContainerChangeData, 0x00485E00, TESObjectREFR*, TESContainer*);
};