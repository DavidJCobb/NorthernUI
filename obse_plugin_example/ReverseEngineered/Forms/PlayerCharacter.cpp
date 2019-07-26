#include "PlayerCharacter.h"

namespace RE {
   bool*  const bPlayerInVanityMode  = (bool*)  0x00B3BB04;
   float* const fVanityCurDistance   = (float*) 0x00B3BB24;
   float* const fVanityControlX      = (float*) 0x00B3BB28;
   float* const fVanityControlY      = (float*) 0x00B3BB20;
   float* const fAutoVanityModeTimer = (float*) 0x00B3BB08;
   float* const fPlayerCameraPitch   = (float*) 0x00B3BAC4;
   float* const fPlayerCameraYaw     = (float*) 0x00B3BAC8;
   bool*  const bChaseCameraResetQueued = (bool*) 0x00B14E4D;
   // 0x00B3BB05 may be an "auto vanity" bool, but auto vanity also sets 0x00B3BB04 to true

   //constexpr float* fPlayerMoveAnimMult = (float*)0x00B14E58;
   //constexpr float* fPlayerTurnAnimMult = (float*)0x00B14E5C;

   __declspec(naked) void ModPCMiscStat(UInt32 statIndex, UInt32 changeBy) {
      //
      // Writing this in C++ can cause the compiler to compile wrong code; sometimes, 
      // (*g_thePlayer)->miscStats[x]++ compiles as (**g_thePlayer)->miscStats[x]++.
      //
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