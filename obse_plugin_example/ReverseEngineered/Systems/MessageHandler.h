#pragma once
#include "obse/Utilities.h"

namespace RE {
   class MessageHandler;
   constexpr MessageHandler*  singletonMessageHandler = (MessageHandler*)  0x00B33F80; // where the handler exists in memory
   constexpr MessageHandler** ptrMessageHandler       = (MessageHandler**) 0x00B34D90; // the variable used to access the handler; if you rely on this, then other people can repoint the object if needed

   class MessageHandler {
      public:
         static MessageHandler* GetInstance() {
            return *ptrMessageHandler;
         };

         virtual void Unk_00(void);
         virtual UInt32 Unk_01(void*); // 01
         virtual void Unk_02(void);
         virtual void Unk_03(void);
         virtual void Unk_04(void);
         virtual void Unk_05(void);
         virtual void Unk_06(const char* message); // 06
         virtual void Unk_07(void);
         virtual void Unk_08(void);
         virtual UInt32 Unk_09(const char* messageText, UInt32 messageBaseButtonIndex, UInt32 options); // 09 // Displays an error? // (options & 0xF) == enum indicating the buttons to show: 2 = Ignore/Abort/Retry; 3 = Yes to All/Yes/No; 4 = Yes/No; else just OK
         virtual void Unk_0A(void);

         // Unk_09 is clearly intended to show warning popups; it seems like it actually 
         // takes over the main loop, i.e. it blocks, and polls input on its own, until 
         // you dismiss the message box it shows. If the window doesn't have focus, it 
         // skips all that and just falls back to WinAPI MessageBoxA.

         UInt8  unk04;
         UInt8  unk05[3];
         UInt32 unk08;
         UInt32 unk0C;
         UInt8  unk10;
         UInt8  unk11[3];

         MEMBER_FN_PREFIX(MessageHandler);
         DEFINE_MEMBER_FN(Subroutine00494410, void, 0x00494410, UInt8); // constructor?
   };
};
