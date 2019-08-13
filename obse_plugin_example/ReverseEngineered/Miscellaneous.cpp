#include "Miscellaneous.h"
#include "obse/GameForms.h"
#include "obse/Script.h"

namespace RE {
   namespace native { // here for documentation purposes; don't bother actually using these
      DEFINE_SUBROUTINE(SInt32, _access, 0x00982D71, const char* path, UInt32); // educated guess
      DEFINE_SUBROUTINE(FILE,   fopen,   0x00982440, const char* filename, const char* mode);
      DEFINE_SUBROUTINE(size_t, fread,   0x00987F5B, void* buffer, size_t elementSize, size_t elementCount, FILE *stream);
      DEFINE_SUBROUTINE(size_t, fread_s, 0x0098817E, void* buffer, size_t bufSize, size_t elementSize, size_t elementCount, FILE *stream); // could be _fread_nolock_s2
      DEFINE_SUBROUTINE(UInt32, fseek,   0x00984801, FILE* stream, SInt32 offset, UInt32 origin);
      DEFINE_SUBROUTINE(void,   _splitpath, 0x0098443C, const char* path, char* outDrive, char* outDirectory, char* outFilename, char* outExtension);
      DEFINE_SUBROUTINE(UInt32, stat,    0x00983754, const char* path, void* out);
      DEFINE_SUBROUTINE(UInt32, strncmp, 0x009864D9, const char*, const char*, size_t);
   };

   DEFINE_SUBROUTINE(const char*, ActorValueIndexToSkillName, 0x00565CC0, UInt32 avIndex);
   DEFINE_SUBROUTINE(bool,   FormIsGoldAsInCurrency, 0x00469980, TESForm* form);
   DEFINE_SUBROUTINE(UInt32, GetFormQuality,         0x0046E3F0, TESForm* form);
   DEFINE_SUBROUTINE(float,  GetFormWeight,          0x00470620, TESForm* form);
   DEFINE_SUBROUTINE(TESBipedModelForm*, GetTESBipedModelForm, 0x004691B0, TESForm* form);
   DEFINE_SUBROUTINE(SInt32, MagicSchoolToActorValueIndex, 0x0041B770, MagicSchool); // returns -1 if invalid
   DEFINE_SUBROUTINE(float,  RoundToMultipleOf,      0x004843B0, float value, float divisor);
   DEFINE_SUBROUTINE(UInt32, SkillMasteryLevelToMinimumSkillLevel, 0x0056A360, UInt32 skillMasteryLevel);
   DEFINE_SUBROUTINE(const char*, SkillMasteryLevelToString, 0x0056A340, UInt32 skillMasteryLevel);

   DEFINE_SUBROUTINE(bool, IsQuietNaN, 0x00984012, double);

   DEFINE_SUBROUTINE(void, MultiplyMatrixByColumn, 0x00710250, NiVector3& out, const NiVector3& column, const NiMatrix33& matrix);

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

namespace cobb {
   void matrix_from_ni(matrix& out, const NiMatrix33& source) {
      static_assert(sizeof(NiMatrix33) == sizeof(matrix), "Structs not the same size.");
      memcpy(&out, &source, sizeof(NiMatrix33));
   };
   void matrix_to_ni(NiMatrix33& out, const matrix& source) {
      static_assert(sizeof(NiMatrix33) == sizeof(matrix), "Structs not the same size.");
      memcpy(&out, &source, sizeof(NiMatrix33));
   };
};