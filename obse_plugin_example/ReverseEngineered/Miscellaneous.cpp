#include "Miscellaneous.h"
#include "obse/GameForms.h"
#include "obse/Script.h"

namespace RE {
   DEFINE_SUBROUTINE(bool,   FormIsGoldAsInCurrency, 0x00469980, TESForm* form);
   DEFINE_SUBROUTINE(UInt32, GetFormQuality,         0x0046E3F0, TESForm* form);
   DEFINE_SUBROUTINE(float,  GetFormWeight,          0x00470620, TESForm* form);
   DEFINE_SUBROUTINE(TESBipedModelForm*, GetTESBipedModelForm, 0x004691B0, TESForm* form);
   DEFINE_SUBROUTINE(float,  RoundToMultipleOf,      0x004843B0, float value, float divisor);
   DEFINE_SUBROUTINE(UInt32, SkillMasteryLevelToMinimumSkillLevel, 0x0056A360, UInt32 skillMasteryLevel);
   DEFINE_SUBROUTINE(const char*, SkillMasteryLevelToString, 0x0056A340, UInt32 skillMasteryLevel);

   bool GetScriptVariableValue(TESQuest* quest, const char* variableName, double& out) {
      out = 0.0;
      //
      auto script = quest->scriptable.script;
      if (!script)
         return false;
      auto varBase = script->GetVariableByName(variableName);
      if (!varBase)
         return false;
      UInt32 varID = varBase->idx;
      auto evList = EventListFromForm(quest);
      if (!evList)
         return false;
      auto varCurrent = evList->GetVariable(varID);
      if (!varCurrent)
         return false;
      out = varCurrent->data;
      return true;
   }
};