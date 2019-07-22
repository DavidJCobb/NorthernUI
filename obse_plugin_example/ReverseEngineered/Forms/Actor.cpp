#include "Actor.h"

namespace RE {
   const char** s_AnimCombatTypeNames = (const char**)0x00B102C8;
   const char** s_AnimBodyPartNames   = (const char**)0x00B108EC;
   const char** s_AnimBodyStateNames  = (const char**)0x00B102B8;
   const char** s_NiControllerSequencCycleTypeNames = (const char**)0x00B02C74; // TODO: MOVE ME
   const char** s_NiControllerSequenceStateNames = (const char**)0x00B02C58; // TODO: MOVE ME
   extern const AnimCombatType* s_WeaponTypeToAnimCombatType = (AnimCombatType*)0x00B086B8;

   DEFINE_SUBROUTINE(bool,           AnimGroupTypeIsIdle,    0x0051AAE0, AnimCode);
   DEFINE_SUBROUTINE(AnimBodyState,  BodyStateFromAnimCode,  0x0051A9D0, AnimCode);
   DEFINE_SUBROUTINE(AnimCombatType, CombatTypeFromAnimCode, 0x0051A9E0, AnimCode);
   DEFINE_SUBROUTINE(AnimCode, MakeAnimCode, 0x0051A9B0, AnimBodyState, AnimCombatType, AnimGroupType);
};