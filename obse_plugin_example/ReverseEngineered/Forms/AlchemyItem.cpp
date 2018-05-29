#include "AlchemyItem.h"

namespace RE {
   AlchemyItem* AlchemyItem::Create() {
      auto result = (AlchemyItem*) FormHeap_Allocate(sizeof(AlchemyItem));
      if (result)
         CALL_MEMBER_FN(result, Constructor)();
      return result;
   };
};