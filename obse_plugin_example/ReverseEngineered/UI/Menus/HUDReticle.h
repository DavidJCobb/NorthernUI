#pragma once
#include "shared.h"
#include "ReverseEngineered/UI/Menu.h"

class TESObjectREFR;
namespace RE {
   class HUDReticle : public Menu {
      public:
         
   };
   extern Tile** HUDReticleTileID1; // reticle
   extern Tile** HUDReticleTileID2; // enemy health
   extern Tile** HUDReticleTileID3; // stealth meter; game prefers a Tile3D but any tile type seems to be safe
   extern bool*  HUDReticleByteUnk1; // set to 0 when HUDReticle is constructed // true if showing tile #3 and false if hiding?
   extern UInt8* HUDReticleByteUnk2; // set to 0 when HUDReticle is constructed
   extern UInt32* HUDReticleDwordUnk1;
   extern float* HUDReticleCachedFadeSpeed1; // fade speed for tile 1
   extern float* HUDReticleCachedFadeSpeed2; // fade speed for tile 2
   extern float* HUDReticleCachedFadeSpeed3; // fade speed for tile 3
   extern float* HUDReticleAlphaCap2; // max alpha for tile 2; varies at run-time

   void UpdateHUDReticleTileOpacities();

   DEFINE_SUBROUTINE_EXTERN(void, SetHUDReticleStateIfPlayerHasNode,   0x00578CF0, UInt8 enumArg); // 0, 1, 2 == hide, show, toggle
   DEFINE_SUBROUTINE_EXTERN(void, SetHUDReticleFocusedRefIfApplicable, 0x00578D50, TESObjectREFR*);
   DEFINE_SUBROUTINE_EXTERN(void, SetHUDReticleIconIfApplicable,       0x00578D30, UInt32 iconEnum);
};