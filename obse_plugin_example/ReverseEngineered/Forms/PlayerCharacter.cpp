#include "PlayerCharacter.h"

namespace RE {
   const bool*  bPlayerInVanityMode = (bool*)  0x00B3BB04;
   const float* fVanityCurDistance  = (float* )0x00B3BB24;
   // 0x00B3BB05 may be an "auto vanity" bool, but auto vanity also sets 0x00B3BB04 to true

   __declspec(naked) void ModPCMiscStat(UInt32 statIndex, UInt32 changeBy) {
      _asm {
         mov  ecx, 0x00B333C4;      // ecx = g_thePlayer;
         mov  ecx, dword ptr [ecx]; // ecx = *g_thePlayer;
         mov  eax, dword ptr [esp + 0x4]; // eax = statIndex;
         mov  edx, dword ptr [esp + 0x8]; // edx = changeBy;
         add  dword ptr [eax * 4 + ecx + 0x658], edx;
         retn;
      }
   };
};