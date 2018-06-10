#pragma once

//
// MenuQue patches calls into the game very sloppily: if calls spill over 
// other vanilla opcodes and "break them in half," MenuQue doesn't NOP the 
// broken bytes. This causes my disassembler to get confused when viewing 
// the patched subroutines and frankly, it's just irritating.
//
// We're gonna check for JMP and CALL instructions at addresses that we 
// know MenuQue patches, and if we find such instructions, we'll write NOP 
// opcodes over the broken bytes, so I can debug my mod and ensure MenuQue 
// compatibility without MenuQue getting in the way of either task.
//

struct MenuQueState {
   bool   detected = false;
   UInt32 addrBase = 0;
   UInt32 addrSize = 0;
   bool   newMenuIDFixFailed = false;
};
extern MenuQueState g_menuQue;

namespace CobbPatches {
   namespace CleanUpAfterMenuQue {
      void Apply();
   }
}