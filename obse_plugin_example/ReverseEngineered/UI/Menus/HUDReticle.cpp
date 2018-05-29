#include "HUDReticle.h"

namespace RE {
   Tile** HUDReticleTileID1 = (Tile**) 0x00B3B350;
   Tile** HUDReticleTileID2 = (Tile**) 0x00B3B358; //
   Tile** HUDReticleTileID3 = (Tile**) 0x00B3B354; // Bethesda's compiler mixed these up -- not me
   bool*  HUDReticleByteUnk1 = (bool*) 0x00B3B361;
   UInt8* HUDReticleByteUnk2 = (UInt8*) 0x00B3B362;
   UInt32* HUDReticleDwordUnk1 = (UInt32*) 0x00B3B368;
   float* HUDReticleAlphaCap2 = (float*) 0x00B140B8;
   float* HUDReticleCachedFadeSpeed1 = (float*) 0x00B140BC;
   float* HUDReticleCachedFadeSpeed2 = (float*) 0x00B140C0;
   float* HUDReticleCachedFadeSpeed3 = (float*) 0x00B140C4;

   __forceinline void UpdateHUDReticleTileOpacities() {
      auto f = (void(*)()) 0x005A82D0;
      f();
   };
   
   DEFINE_SUBROUTINE(void, SetHUDReticleStateIfPlayerHasNode,   0x00578CF0, UInt8 enumArg); // 0, 1, 2 == hide, show, toggle
   DEFINE_SUBROUTINE(void, SetHUDReticleFocusedRefIfApplicable, 0x00578D50, TESObjectREFR*);
   DEFINE_SUBROUTINE(void, SetHUDReticleIconIfApplicable,       0x00578D30, UInt32 iconEnum);
};