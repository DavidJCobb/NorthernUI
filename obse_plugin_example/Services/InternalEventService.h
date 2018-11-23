#pragma once
#include <vector>;

namespace RE {
   class MapMenu;
}
class InternalEventService {
   //
   // Service to allow parts of this DLL to react to the same thing, share a 
   // hook, etc., etc..
   //
   public:
      typedef void (*Callback_DrawWorldMap)(RE::MapMenu*);
      enum CallbackType {
         kCallbackType_OnAfterWorldMapMarkersDrawn,
      };
   private:
      struct {
         std::vector<Callback_DrawWorldMap> onAfterWorldMapMarkersDrawn;
      } callbacks;
   public:
      static InternalEventService& GetInstance() {
         static InternalEventService instance;
         return instance;
      }
      //
      template<typename T> void RegisterCallback(T callback) {};
      template<> void RegisterCallback(Callback_DrawWorldMap callback) {
         this->callbacks.onAfterWorldMapMarkersDrawn.push_back(callback);
      }
      void FireCallback(CallbackType, ...) const;
};