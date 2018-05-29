#include "EffectItem.h"
#include "obse/GameAPI.h"

namespace RE {
   EffectItem* EffectItem::Clone(EffectItem* source) {
      auto result = (EffectItem*) FormHeap_Allocate(sizeof(EffectItem));
      if (result)
         CALL_MEMBER_FN(result, CopyConstructor)(source);
      return result;
   };
   EffectItem* EffectItem::Create(EffectSetting* setting) {
      auto result = (EffectItem*) FormHeap_Allocate(sizeof(EffectItem));
      if (result)
         CALL_MEMBER_FN(result, Constructor)(setting);
      return result;
   };
   bool EffectItemList::HasEffects() {
      auto entry = &this->effectList;
      do {
         if (entry->effectItem)
            return true;
      } while (entry = entry->next);
      return false;
   };
};