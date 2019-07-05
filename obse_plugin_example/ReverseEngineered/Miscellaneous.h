#pragma once
#include "ReverseEngineered/_BASE.h"
#include "obse/Utilities.h"

class TESBipedModelForm;
class TESQuest;
namespace RE {
   enum MagicSchool : UInt32 { // AVs 0x14 - 0x19
      kMagicSchool_Alteration  = 0,
      kMagicSchool_Conjuration = 1,
      kMagicSchool_Destruction = 2,
      kMagicSchool_Enchanting  = 3,
      kMagicSchool_Illusion    = 4,
   };
   enum SkillMasteryLevel {
      kSkillMastery_Novice = 0,
      kSkillMastery_Apprentice = 1,
      kSkillMastery_Journeyman = 2,
      kSkillMastery_Expert = 3,
      kSkillMastery_Master = 4, // Bethesda checks with >= so you probably should as well
   };

   DEFINE_SUBROUTINE_EXTERN(const char*, ActorValueIndexToSkillName, 0x00565CC0, UInt32 avIndex);
   DEFINE_SUBROUTINE_EXTERN(bool,   FormIsGoldAsInCurrency, 0x00469980, TESForm* form);
   DEFINE_SUBROUTINE_EXTERN(UInt32, GetFormQuality,         0x0046E3F0, TESForm* form);
   DEFINE_SUBROUTINE_EXTERN(float,  GetFormWeight,          0x00470620, TESForm* form);
   DEFINE_SUBROUTINE_EXTERN(UInt32, GetSkillMasteryLevel,   0x0056A300, UInt32 skillLevel);
   DEFINE_SUBROUTINE_EXTERN(TESBipedModelForm*, GetTESBipedModelForm, 0x004691B0, TESForm* form);
   DEFINE_SUBROUTINE_EXTERN(SInt32, MagicSchoolToActorValueIndex, 0x0041B770, MagicSchool); // returns -1 if invalid
   DEFINE_SUBROUTINE_EXTERN(float,  RoundToMultipleOf,      0x004843B0, float value, float divisor);
   DEFINE_SUBROUTINE_EXTERN(UInt32, SkillMasteryLevelToMinimumSkillLevel, 0x0056A360, UInt32 skillMasteryLevel);
   DEFINE_SUBROUTINE_EXTERN(const char*, SkillMasteryLevelToString, 0x0056A340, UInt32 skillMasteryLevel);

   struct NiMatrix33 {
      //
      // [0, 1, 2,
      //  3, 4, 5,
      //  6, 7, 8]
      //
      float	data[9];

      float& operator[] (int x) {
         return this->data[x];
      }

      MEMBER_FN_PREFIX(NiMatrix33);
      DEFINE_MEMBER_FN(AddViaCopy,       NiMatrix33*, 0x0070FFC0, NiMatrix33* out, NiMatrix33* other); // returns out
      DEFINE_MEMBER_FN(MultiplyByColumn, NiVector3*,  0x007101F0, NiVector3*  out, NiVector3*  other); // returns out
      DEFINE_MEMBER_FN(MultiplyViaCopy,  NiMatrix33*, 0x007100A0, NiMatrix33* out, NiMatrix33* other); // returns out
      DEFINE_MEMBER_FN(SetFromPitch,     void,        0x0070FD30, float pitch); // same unit as TESObjectREFR::rotX
      DEFINE_MEMBER_FN(SetFromYaw,       void,        0x0070FDD0, float yaw); // same unit as TESObjectREFR::rotZ
      DEFINE_MEMBER_FN(SetToIdentity,    void,        0x0070FD10);
      DEFINE_MEMBER_FN(SubtractViaCopy,  NiMatrix33*, 0x00710030, NiMatrix33* out, NiMatrix33* other); // returns out
   };

   struct SimpleLock {
      UInt32 threadID;
      UInt32 counter;
   };

   bool GetScriptVariableValue(TESQuest* quest, const char* variableName, double& out);
};