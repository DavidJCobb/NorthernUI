#include "PlayerCharacter.h"
#include "obse/NiRenderer.h"

namespace RE {
   bool*  const bPlayerInVanityMode  = (bool*)  0x00B3BB04;
   float* const fVanityCurDistance   = (float*) 0x00B3BB24;
   float* const fVanityControlX      = (float*) 0x00B3BB28;
   float* const fVanityControlY      = (float*) 0x00B3BB20;
   float* const fAutoVanityModeTimer = (float*) 0x00B3BB08;
   float* const fPlayerCameraPitch   = (float*) 0x00B3BAC4;
   float* const fPlayerCameraYaw     = (float*) 0x00B3BAC8;
   bool*  const bCameraInertiaResetQueued = (bool*) 0x00B14E4D;
   DetectionState* const iPlayerCurrentDetectionState = (DetectionState*) 0x00B3B368;
   // 0x00B3BB05 may be an "auto vanity" bool, but auto vanity also sets 0x00B3BB04 to true
   
   float* const fPlayerMoveAnimMult = (float*)0x00B14E58; // also affects movement speed
   float* const fPlayerTurnAnimMult = (float*)0x00B14E5C; // for Q/W keys

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

   bool PlayerCharacter::IsOblivionReloadedImmersiveFirstPerson() const {
      //
      // Oblivion Reloaded's "immersive first-person camera" feature works by 
      // making it so that you're always in third-person view, with the game 
      // positioning the camera inside of your head. This means that the 
      // (isThirdPerson) field on PlayerCharacter is no longer reliable.
      //
      // Oblivion Reloaded expands NiDX9Renderer from 0xB00 bytes to 0xB44 
      // bytes, and uses the bool at offset 0xB41 to indicate whether we're 
      // in true third-person, or third-person as a proxy for immersive first-
      // person.
      //
      // UPDATE: As of Oblivion Reloaded version 8.0.0, NiDX9Renderer is 
      // expanded to an even larger size, and the bool we want has moved.
      //
      UInt32 rendererSize = *(UInt32*)0x0076BD75;
      if (rendererSize >= 0xB44) {
         //
         // OBSE has shimmed the NiDX9Renderer instance.
         //
         bool value;
         auto renderer = (uint8_t*)(g_renderer);
         if (rendererSize > 0xB44) {
            value = *(renderer + 0xB82);
         } else {
            value = *(renderer + 0xB41);
         }
         return value;
      }
      return false;
   }
};