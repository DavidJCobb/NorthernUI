#pragma once

namespace RE {
   class Menu;

   namespace MQ {
      // Accessor for a vanilla VTBL; cast a pointer to the VTBL to a pointer to this
      struct MQVtbl {
         void* data;

         // $+0FF20 is a member function, void(UInt32 offset, void* func), that overwrites a function in the VTBL
         // where offset = sizeof(void*) * index
      };

      // MenuQue has an array of these indicating info about all menus
      struct MenuInfo { // sizeof == 0x34
         const char* name; // 00
         UInt32      id;   // 04
         UInt32      unk08;
         MQVtbl      vtbl; // 0C
         void*  pendingOriginalCall; // 10 // can get set to originalHandleMouseUp
         UInt32 unk14; // 14
         void*  originalHandleFrame;   // 18
         void*  originalHandleMouseUp; // 1C // pointer directly to the member function; you must set (this) before calling; nullptr if MenuQue hasn't replaced the function
         void*  originalHandleMouseover; // 20
         void*  originalHandleFrameMouseWheel; // 24
         void*  originalHandleFrameMouseDown;  // 28
         void*  originalHandleKeyboardInput;   // 2C
         Menu*  currentInstance; // 30
      };
      // mq_g_menuInfo == array of MenuInfo is at $+4C380; has 0x2E elements
   }
}