#pragma once
#include "obse/GameData.h"
#include "obse/GameForms.h"
#include "ReverseEngineered/Miscellaneous.h"

namespace RE {
   typedef ModEntry::Data TESFile;
   struct BSAEntry;

   // 0x00450C20 TESFile::GetCString(char&, UInt32 zero)

   struct ModelTextureFileHashList { // list items in MODT and NIFT
      UInt8  length = 0;
      UInt8  pad01[3];
      BSAEntry** unk04 = nullptr; // array of BSAEntry* pointers

      MEMBER_FN_PREFIX(ModelTextureFileHashList);
      DEFINE_MEMBER_FN(Destructor, void, 0x0046D450);
      DEFINE_MEMBER_FN(LoadData, void, 0x0046D610, UInt8* data, UInt8 length, TESForm*, UInt32); // NIFT
      DEFINE_MEMBER_FN(LoadMODT, void, TESFile*, TESForm* owner, const char* path); // MODT, MO2T, MO3T, MO4T
      DEFINE_MEMBER_FN(Subroutine0046D610, void, 0x0046D610, void* fileData, UInt32 maybeCount, TESForm* owner, const char* path); // MODT, MO2T, MO3T, MO4T

      //
      // NIFT SUBRECORD CONTENTS:
      // UInt32 subListCount
      // UInt8  subList1Length
      // UInt8  bytes[subList1Length * 0x24]
      // UInt8  subList2Length
      // UInt8  bytes[subList2Length * 0x24]
      // UInt8  subList3Length
      // UInt8  bytes[subList3Length * 0x24]
      // ...
      //
      // List items in this case appear to be a 64-bit filename hash, eight unused bytes, and a 
      // 64-bit folder name hash, but this is hard to verify because I don't know of any records 
      // that have a (non-zero-length) NIFT subrecord, and the data isn't actually retained in 
      // memory: LoadData uses the hashes to grab BSAEntry pointers via LazyFileLookup and then 
      // discards the hashes.
      //
      // I'm told that this is an array counterpart to MODT, which also uses 24-byte elements.
      //
      // Incidentally, NIFT triggers the loose file problems that BSA redirection fixes, because 
      // it doesn't keep file paths for the textures involved; this means that when it performs 
      // the lazy file lookup, it can't supply the path for a loose file check.
      //
   };

   class TESModel : public BaseFormComponent { // sizeof == 0x18
      public:
         TESModel();
         ~TESModel();

         virtual void			Unk_03(void);
         virtual void			Unk_04(void);
         virtual const char*	GetModelPath(void);
         virtual void			SetModelPath(const char* path);

         BSStringT nifPath;    // 04
         float     editorSize; // 0C 'size' field in editor, from MODB subrecord. Only present for objects defined in Oblivion.esm?
         ModelTextureFileHashList unk10; // 10
   };

   DEFINE_SUBROUTINE_EXTERN(void, HandleMODTAndVariants, 0x0046D490, TESModel*, TESFile*);
};