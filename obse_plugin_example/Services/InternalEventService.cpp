#include "InternalEventService.h"

void InternalEventService::FireCallback(CallbackType ct, ...) const {
   va_list args;
   va_start(args, ct);
   switch (ct) {
      case kCallbackType_OnAfterWorldMapMarkersDrawn:
         {
            auto  menu = va_arg(args, RE::MapMenu*);
            auto& list = this->callbacks.onAfterWorldMapMarkersDrawn;
            auto  size = list.size();
            for (size_t i = 0; i < size; i++)
               list[i](menu);
         }
         break;
   }
   va_end(args);
}