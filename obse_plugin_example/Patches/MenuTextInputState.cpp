#include "MenuTextInputState.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace MenuTextInputState {
      namespace UpDownArrowFix {
         //
         // There is a bug in MenuTextInputState::HandleKeypress. A switch-case is performed on the 
         // input char, and if it matches certain special character codes such as the arrow keys, 
         // then special behaviors are performed. The default case is to treat the input char as a 
         // real text character, and write it to the edit value.
         //
         // Unfortunately, Bethesda didn't define cases for the up and down arrow keys. As such, 
         // they result in garbage characters (charcodes 02 and 03) being written to the edit value. 
         // 
         // Fortunately, there's an easy fix. This switch-case was compiled as a table of pointers: 
         // if we're dealing with a special-case code (it's less than 0A), the game simply reads the 
         // value at ptrSwitchCaseTable[code] and jumps to it. As such, we can just copy the pointers 
         // for the Home and End keys over the pointers for the Up Arrow and Down Arrow keys.
         //
         UInt32* ptrSwitchCaseTable = (UInt32*) 0x005800F0;

         void Apply() {
            SafeWrite32((UInt32) &ptrSwitchCaseTable[3], ptrSwitchCaseTable[5]);
            SafeWrite32((UInt32) &ptrSwitchCaseTable[4], ptrSwitchCaseTable[6]);
         };
      };

      void Apply() {
         UpDownArrowFix::Apply();
      };
   }
};