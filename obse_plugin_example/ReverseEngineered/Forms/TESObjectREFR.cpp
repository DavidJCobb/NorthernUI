#include "TESObjectREFR.h"

bool _ItemIsPlayable(TESForm* base) {
   if (!base)
      return false;
   switch (base->typeID) {
      case kFormType_LeveledItem:
         //
         // Leveled items are not playable; rather, they lead to the spawning of real items 
         // which are playable. Think of leveled items as spawn points that are inside of 
         // an inventory; the leveled items exist within the base TESContainer and the real 
         // items exist within ExtraContainerChanges.
         //
         return false;
      case kFormType_Book:
         return (((TESObjectBOOK*) base)->bookFlags & TESObjectBOOK::kBook_CantBeTaken) == 0;
      case kFormType_Armor:
         return (((TESObjectARMO*) base)->bipedModel.flags & TESBipedModelForm::kFlags_NotPlayable) == 0;
      case kFormType_Clothing:
         return (((TESObjectCLOT*) base)->bipedModel.flags & TESBipedModelForm::kFlags_NotPlayable) == 0;
   }
   return true;
};

namespace RE {
   DEFINE_SUBROUTINE(ExtraContainerChanges::Data*, GetContainerChangeData, 0x00485E00, TESObjectREFR*, TESContainer*);

   bool TESObjectREFR::IsEmpty() {
      SInt32 playableCount = 0;
      auto container = CALL_MEMBER_FN(this, GetBaseTESContainer)();
      if (!container)
         return false; // a non-container is neither empty nor full
      {  // Count items in container
         TESContainer::Entry* entry = &container->list;
         do {
            auto data = entry->data;
            if (data && _ItemIsPlayable(data->type))
               playableCount += data->count;
         } while (entry = entry->next);
      }
      {  // Count items in changes (can be negative if default items have been removed)
         auto changes = GetContainerChangeData(this, container);
         for (auto entry = changes->objList; entry; entry = entry->next) {
            auto data = entry->data;
            if (data && _ItemIsPlayable(data->type))
               playableCount += data->countDelta;
         }
      }
      return playableCount <= 0;
   };
};