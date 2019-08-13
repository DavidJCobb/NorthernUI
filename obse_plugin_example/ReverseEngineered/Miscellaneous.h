#pragma once
#include "ReverseEngineered/_BASE.h"
#include "obse/Utilities.h"

#include "Miscellaneous/rotation.h"

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

   namespace native { // here for documentation purposes; don't bother actually using these
      DEFINE_SUBROUTINE_EXTERN(SInt32, _access, 0x00982D71, const char* path, UInt32); // educated guess
      DEFINE_SUBROUTINE_EXTERN(FILE,   fopen,   0x00982440, const char* filename, const char* mode);
      DEFINE_SUBROUTINE_EXTERN(size_t, fread,   0x00987F5B, void* buffer, size_t elementSize, size_t elementCount, FILE *stream);
      DEFINE_SUBROUTINE_EXTERN(size_t, fread_s, 0x0098817E, void* buffer, size_t bufSize, size_t elementSize, size_t elementCount, FILE *stream);
      DEFINE_SUBROUTINE_EXTERN(UInt32, fseek,   0x00984801, FILE* stream, SInt32 offset, UInt32 origin);
      DEFINE_SUBROUTINE_EXTERN(void,   _splitpath, 0x0098443C, const char* path, char* outDrive, char* outDirectory, char* outFilename, char* outExtension);
      DEFINE_SUBROUTINE_EXTERN(UInt32, stat,    0x00983754, const char* path, void* out);
      DEFINE_SUBROUTINE_EXTERN(UInt32, strncmp, 0x009864D9, const char*, const char*, size_t);

      static constexpr UInt32* CONST_SEEK_SET = (UInt32*)0x00A853D0;
      static constexpr UInt32* CONST_SEEK_CUR = (UInt32*)0x00A853D4;
      static constexpr UInt32* CONST_SEEK_END = (UInt32*)0x00A853D8;
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

   DEFINE_SUBROUTINE_EXTERN(bool, IsQuietNaN, 0x00984012, double);

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
      const float& operator[] (int x) const {
         return this->data[x];
      }

      MEMBER_FN_PREFIX(NiMatrix33);
      DEFINE_MEMBER_FN(ApplyRelativeRotation, NiMatrix33*, 0x0053D7A0, NiMatrix33& out, const NiMatrix33& relative); // returns out
      DEFINE_MEMBER_FN(AddViaCopy,       NiMatrix33*, 0x0070FFC0, NiMatrix33& out, const NiMatrix33& other); // returns out
      DEFINE_MEMBER_FN(Equals,           bool,        0x0070FF20, const NiMatrix33& other);
      DEFINE_MEMBER_FN(Invert,           bool,        0x007102B0, NiMatrix33& out); // returns false if the operation fails
      DEFINE_MEMBER_FN(InvertForce,      void,        0x007103C0, NiMatrix33& out); // if the operation fails, sets the out matrix to all zeroes
      DEFINE_MEMBER_FN(MultiplyByColumn, NiVector3*,  0x007101F0, NiVector3&  out, const NiVector3& other); // returns out
      DEFINE_MEMBER_FN(MultiplyByScalar, NiMatrix33*, 0x00710190, NiMatrix33& out, float scalar); // returns out
      DEFINE_MEMBER_FN(MultiplyViaCopy,  NiMatrix33*, 0x007100A0, NiMatrix33& out, const NiMatrix33& other); // out = this * other // returns out
      DEFINE_MEMBER_FN(MultiplyOpposite, NiMatrix33*, 0x00710490, NiMatrix33& out, const NiMatrix33& other); // out = other * this // returns out // matrix multiplication is not commutative
      DEFINE_MEMBER_FN(Overwrite,        NiMatrix33*, 0x00710400, NiMatrix33& out); // overwrites (out) with (this)
      DEFINE_MEMBER_FN(SetFromAxisAngle, void,        0x0070FE20, float angle, float x, float y, float z);
      DEFINE_MEMBER_FN(SetFromColumns,   NiMatrix33*, 0x0070FCC0, const NiVector3& col1, const NiVector3& col2, const NiVector3& col3); // returns self
      DEFINE_MEMBER_FN(SetFromPitch,     void,        0x0070FD30, float pitch); // same unit as TESObjectREFR::rotX
      DEFINE_MEMBER_FN(SetFromRoll,      void,        0x0070FD80, float roll);
      DEFINE_MEMBER_FN(SetFromYaw,       void,        0x0070FDD0, float yaw); // same unit as TESObjectREFR::rotZ
      DEFINE_MEMBER_FN(SetToIdentity,    void,        0x0070FD10);
      DEFINE_MEMBER_FN(SubtractViaCopy,  NiMatrix33*, 0x00710030, NiMatrix33& out, const NiMatrix33& other); // returns out

      inline bool operator==(const NiMatrix33& other) {
         return CALL_MEMBER_FN(this, Equals)(other);
      }

      float determinant() const {
         const NiMatrix33& m = *this; // we can't stop operator[] on NiMatrix33* from being treated as pointer index
         return (m[0] * m[4] * m[8]) +
                (m[1] * m[5] * m[6]) +
                (m[2] * m[3] * m[7]) -
                (m[2] * m[4] * m[6]) -
                (m[1] * m[3] * m[8]) -
                (m[0] * m[5] * m[7]);
      }

      NiMatrix33 transpose() const noexcept {
         NiMatrix33 out;
         out.data[0] = this->data[0];
         out.data[1] = this->data[3];
         out.data[2] = this->data[6];
         out.data[3] = this->data[2];
         out.data[4] = this->data[4];
         out.data[5] = this->data[7];
         out.data[6] = this->data[2];
         out.data[7] = this->data[5];
         out.data[8] = this->data[8];
         return out;
      };
   };

   //
   // out.x = (other.x * mat[0]) + (other.y * mat[3]) + (other.z * mat[6]);
   // out.y = (other.x * mat[1]) + (other.y * mat[4]) + (other.z * mat[7]);
   // out.z = (other.x * mat[2]) + (other.y * mat[5]) + (other.z * mat[8]);
   //
   DEFINE_SUBROUTINE_EXTERN(void, MultiplyMatrixByColumn, 0x00710250, NiVector3& out, const NiVector3& column, const NiMatrix33& matrix);

   struct SimpleLock {
      UInt32 threadID;
      UInt32 counter;
   };

   bool GetScriptVariableValue(TESQuest* quest, const char* variableName, double& out);
};

namespace cobb {
   void matrix_from_ni(matrix& out, const NiMatrix33& source);
   void matrix_to_ni(NiMatrix33& out, const matrix& source);
};