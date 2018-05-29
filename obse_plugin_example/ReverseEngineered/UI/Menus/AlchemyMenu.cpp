#include "AlchemyMenu.h"

namespace RE {
   UInt32* const AlchemyMenuClickedIngredient = (UInt32*) 0x00B3B270;

   DEFINE_SUBROUTINE(void,  CloseAlchemyMenu, 0x005932B0);
   DEFINE_SUBROUTINE(UInt8, AlchemySkillLevelToRevealedEffectCount, 0x0041BA00, UInt32 skill);
   DEFINE_SUBROUTINE(float, ApplyMortarAndPestleMult,               0x00548400, float mortalAndPestleQuality, float playerLuckModifiedSkill);

   DEFINE_SUBROUTINE(void,  ComputeType1PotionEffectStrength, 0x00548420, float* outMagnitude, float* outDuration, float baseCost, float mortarAndPestleInfluence, UInt32, bool isHostile, float, float, float, bool isNotPoison);
   DEFINE_SUBROUTINE(void,  ComputeType2PotionEffectStrength, 0x005486A0, float* outDuration,                      float baseCost, float mortarAndPestleInfluence, UInt32, bool isHostile, float, float, float, bool isNotPoison);
   DEFINE_SUBROUTINE(void,  ComputeType3PotionEffectStrength, 0x005487F0, float* outMagnitude,                     float baseCost, float mortarAndPestleInfluence, UInt32, bool isHostile, float, float, float, bool isNotPoison);
};