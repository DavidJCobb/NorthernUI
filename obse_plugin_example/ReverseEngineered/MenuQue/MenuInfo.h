#pragma once

namespace RE {
   class Menu;

   namespace MQ {
      enum MenuEventHandlers {
         kMenuEventHandler_Click     = 0, // Menu::HandleMouseUp
         kMenuEventHandler_Mouseover = 1, // Menu::HandleMouseover
         kMenuEventHandler_TextEdit  = 2,
         kMenuEventHandler_TextEditDone = 3,
         kMenuEventHandler_TextEditInput = 4,
         kMenuEventHandler_Open = 5, // relies on shimming Menu::EnableMenu
         kMenuEventHandler_Close = 6,
         kMenuEventHandler_Reload = 7,
         kMenuEventHandler_Scroll = 8, // Menu::HandleMouseWheelFrame
         kMenuEventHandler_Drag = 9, // Menu::HandleMouseDownFrame
         kMenuEventHandler_Input = 10, // Menu::HandleKeyboardInput
         kMenuEventHandler_Update = 11, // Menu::HandleFrame
      };

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