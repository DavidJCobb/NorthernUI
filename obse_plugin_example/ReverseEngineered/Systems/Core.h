#pragma once
#include "obse/Utilities.h"

namespace RE {
   extern bool* const bShowingWindowsCursor;

   class OSInputGlobals;
   class OSSoundGlobals;
   class OSGlobals {
      public:
         OSGlobals();
         ~OSGlobals();

         static OSGlobals* GetInstance() {
            return *(OSGlobals**)(0x00B33398);
         };

         bool  quitQueued;           // 00
         bool  quitToMainMenuQueued; // 01
         bool  windowHasFocus;       // 02
         UInt8 unk03;    // 03
         UInt8 unk04;    // 04
         UInt8 pad05[3]; // 05
         HWND  window;   // 08
         HINSTANCE procInstance;     // 0C
         UInt32    mainThreadID;     // 10
         HANDLE    mainThreadHandle; // 14
         UInt32    unk18; // 18
         UInt32    unk1C; // 1C
         OSInputGlobals* input; // 20
         OSSoundGlobals* sound; // 24

         MEMBER_FN_PREFIX(OSGlobals);
         DEFINE_MEMBER_FN(HandleMouseMove, bool, 0x00405370, UInt16 x, UInt16 y); // WM_MOUSEMOVE handler; in vanilla, if it returns false, the event falls through to the DefWindowProc
   };
   static_assert(sizeof(OSGlobals) >= 0x28, "RE::OSGlobals is too small!");
   static_assert(sizeof(OSGlobals) <= 0x28, "RE::OSGlobals is too large!");

   // Offsets of interest:
   // 0051D6FB and 0052523D are related to input handling for yielding
   // 005799AF and 00579E51 are related to input handling for UI
   // 0057C258 is related to showing control names and bindings in the Controls menu?
   // Subroutine 0057DC60 does stuff with input
   // InterfaceManager::Subroutine0057E7C0 does stuff with input
   // InterfaceManager::Subroutine005821F0 does stuff with input and may be connected to mouse behaviors
   // 0059BE0A checks bindings and may be for typing text
   // 0059C679 seems to read the user's saved controls from Oblivion.ini
   // 005E96E0 plays various UI-related up/down sounds

   // 005ACFF5 and 00600B6C play sound files by path
};