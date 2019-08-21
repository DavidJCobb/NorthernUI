#pragma once
#include "obse/GameData.h"
#include "obse/GameForms.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/Forms/Components/TESModel.h"

namespace RE {
   class TESModelList : public BaseFormComponent { // sizeof == 0x14
      public:
      TESModelList();
      ~TESModelList();

      struct Array {
         UInt32 length; // 00
         ModelTextureFileHashList* data; // 04

         MEMBER_FN_PREFIX(Array);
         DEFINE_MEMBER_FN(clear, void, 0x0046DE10);
         DEFINE_MEMBER_FN(LoadNIFTSubrecord, void, 0x0046DE60, UInt32, TESFile* file);
         //
         // destructor for array element is 0046D450
      };

      LinkedPointerList<const char*> modelPaths; // 04 // subrecord NIFZ
      Array unk0C; // 0C // subrecord NIFT

      MEMBER_FN_PREFIX(TESModelList);
      DEFINE_MEMBER_FN(AddModelPath, void, 0x0046DD70, const char*);
      DEFINE_MEMBER_FN(Load, void, 0x0046DFE0, UInt32, TESFile* file);
   };
};