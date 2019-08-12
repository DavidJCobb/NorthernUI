#pragma once
#include "BSFile.h"
#include "shared.h"
#include "ReverseEngineered/_BASE.h"

namespace RE {
   struct BSHash {
      UInt64 data;

      MEMBER_FN_PREFIX(BSHash);
      DEFINE_MEMBER_FN(Constructor,        BSHash&, 0x006FA2D0, const char* str, UInt32 type);
      DEFINE_MEMBER_FN(Subroutine0042BC10, UInt32,  0x0042BC10, const BSHash& other);
   };

   class Archive;
   constexpr UInt32 ce_bsaSignatureBSwapped = '\0ASB';

   extern LinkedPointerList<Archive>* const g_archiveList; // All loaded BSAs in this order: INI-specified archives; non-Oblivion.esm archives in the same order as the load order. Oblivion BSAs are specified by the INI (though Mod Organizer can override this in a way that looks confusing).
   extern NiTArray<BSHash>* const g_archiveInvalidatedFilenames; // related to archive invalidation bugs. one for directory names; one for file names; which is which?
   extern NiTArray<BSHash>* const g_archiveInvalidatedDirectoryPaths;

   // don't know if this is safe to call:
   DEFINE_SUBROUTINE_EXTERN(void, LoadBSAFile, 0x0042F4C0, const char* filePath, UInt32 zeroA, UInt32 zeroB); // adds archive to g_archiveList
   //
   DEFINE_SUBROUTINE_EXTERN(void, ReadArchiveInvalidationTxtFile, 0x0042D840, const char* filename);

   enum BSAFlags : UInt32 {
      kBSAFlag_HasFolderNames = 0x0001,
      kBSAFlag_HasFileNames   = 0x0002,
      kBSAFlag_Compressed     = 0x0004,
      kBSAFlag_Unk0008        = 0x0008,
      kBSAFlag_RetainFilenameStrings = 0x0010,
      kBSAFlag_RetainFilenameOffsets = 0x0020,
      kBSAFlag_IsXbox360Archive      = 0x0040,
      kBSAFlag_Unk0080         = 0x0080,
      kBSAFlag_Unk0100         = 0x0100,
      kBSAFlag_Unk0200         = 0x0200,
      kBSAFlag_Unk0400         = 0x0400,
   };
   enum BSAFiletypeFlags : UInt32 {
      kBSAFiletypeFlag_Meshes   = 0x0001,
      kBSAFiletypeFlag_Textures = 0x0002,
      kBSAFiletypeFlag_Menus    = 0x0004,
      kBSAFiletypeFlag_Sounds   = 0x0008,
      kBSAFiletypeFlag_Voices   = 0x0010,
      kBSAFiletypeFlag_Shaders  = 0x0020,
      kBSAFiletypeFlag_Trees    = 0x0040,
      kBSAFiletypeFlag_Fonts    = 0x0080,
      kBSAFiletypeFlag_Misc     = 0x0100,
   };
   enum BSAFileFlags : UInt32 { // consumes bits from the BSAEntry::size field
      kBSAFileFlags_NonDefaultCompression = 0x40000000,
      kBSAFileFlags_Unk80000000 = 0x80000000, // possibly "is invalid" // wait, is this in BSAEntry::size or BSAEntry::offset? 0042E4B3 has it in the latter
   };

   struct BSAEntry { // "File Record" or "Folder Record" in UESP docs
      BSHash hash; // 00 // hash of the file/folder path
      union {
         UInt32 count = 0; // number of files
         UInt32 size;      // size of the file
      }; // 08
      union {
         UInt32    offset = 0; // data offset within the file (where it's an offset FROM differs between folders and files)
         BSAEntry* files;      // array of file entries for this folder, once the folder is loaded (happens in Archive constructor)
      }; // 0C

      MEMBER_FN_PREFIX(BSAEntry);
      DEFINE_MEMBER_FN(Constructor, BSAEntry&, 0x0042BD20);

      bool isInvalidated() const {
         return (this->offset & 0x80000000) == 0;
      }
      void invalidate() {
         this->offset &= 0x80000000;
      }
   };
   struct BSAHeader {
      UInt32 unk00 = ce_bsaSignatureBSwapped; // 00
      UInt32 version = 0x67; // 04
      UInt32 offset  = 0x24; // 08
      UInt32 flags   = 0; // 0C // offset 0x160 in Archive
      UInt32 directoryCount = 0; // 10 // offset 0x164 in Archive
      UInt32 fileCount      = 0; // 14 // offset 0x168 in Archive
      UInt32 totalFolderNameLength = 0; // 18 // offset 0x16C in Archive
      UInt32 totalFileNameLength = 0; // 1C // offset 0x170 in Archive
      UInt16 fileFlags = 0; // 20 // offset 0x174 in Archive // BSAFiletypeFlags
      UInt16 pad22; // 22

      MEMBER_FN_PREFIX(BSAHeader);
      DEFINE_MEMBER_FN(Constructor, BSAHeader&, 0x006FA180);
   };

   class Archive : public BSFile { // sizeof == 0x280
      public:
         BSAHeader header; // 154
         BSAEntry* folders = nullptr; // 178 // array
         UInt32 unk17C;
         UInt32 unk180;
         UInt32 unk184;
         UInt32 unk188 = 0; // same type as unk148
         UInt32 unk18C = -1;
         UInt32 unk190 = -1;
         UInt8  unk194 = 0; // initialized to 0 or 8
         UInt8  unk195;
         UInt8  unk196;
         UInt8  unk197;
         char*   folderNames       = nullptr; // 198 // a bunch of consecutive zero-terminated strings
         UInt32* folderNameOffsets = nullptr; // 19C // possibly an array of offsets, in 198, for each folder name
         char*   fileNames   = nullptr; // 1A0 // "File Name Block" in UESP docs: a bunch of consecutive zero-terminated strings
         UInt32** fileNameOffsetsByFolder = nullptr; // 1A4
         UInt32 unk1A8 = 0;
         UInt8  unk1AC = 0;
         UInt8  unk1AD;
         UInt8  unk1AE;
         UInt8  unk1AF;
         UInt32 unk1B0;
         UInt32 unk1B4[(0x200 - 0x1B4) / 4];
         CRITICAL_SECTION unk200; // 200 // wrapped in a struct that allows for automatic initialization/destruction
         UInt32 unk218[(0x280 - 0x218) / 4];

         const char* getFileName(UInt8 folderIndex, UInt8 fileIndex) const {
            auto offset = this->fileNameOffsetsByFolder[folderIndex][fileIndex];
            return &this->fileNames[offset];
         }
         const char* getFolderName(UInt8 index) const {
            auto offset = this->folderNameOffsets[index];
            return &this->folderNames[offset];
         };

         MEMBER_FN_PREFIX(Archive);
         DEFINE_MEMBER_FN(Constructor, Archive&, 0x0042EE80, const char* filePath, UInt32, bool, UInt32);
         DEFINE_MEMBER_FN(RetainsFilenameStringTable, bool, 0x0042BD30);
         DEFINE_MEMBER_FN(RetainsFilenameOffsetTable, bool, 0x0042BD50);
         DEFINE_MEMBER_FN(Subroutine0042BD70, bool, 0x0042BD70);
         DEFINE_MEMBER_FN(Subroutine0042CE40, bool, 0x0042CE40, UInt32, UInt32, const char* normalizedFilepath);
         DEFINE_MEMBER_FN(Subroutine0042D000, bool, 0x0042D000, UInt32, UInt32, UInt32, UInt32, const char* normalizedFilepath);
         DEFINE_MEMBER_FN(Subroutine0042E020, bool, 0x0042E020, UInt32, UInt32, UInt32, UInt32, const char* normalizedFilepath);
         //
         // The below are all called during the constructor, so don't use them:
         //
         DEFINE_MEMBER_FN(InvalidateOlderFiles, UInt32, 0x0042E2D0); // returns number of files invalidated
         DEFINE_MEMBER_FN(InvalidateAgainstLooseFiles, UInt32, 0x0042D2A0, const char* pathRoot, const char* pathDeep, FILETIME& myLastModDate); // called by InvalidateOlderFiles
         DEFINE_MEMBER_FN(LoadFolderNames,      const char*, 0x0042CAE0, UInt32 stopAt); // returns loaded folder names
         DEFINE_MEMBER_FN(SetReadWriteFuncs,    void,   0x004307F0, bool);

         //
         // Don't actually call these; they're offered as an example:
         //
         inline UInt32 _example_InvalidateAgainstLooseFiles() {
            WIN32_FIND_DATAA data;
            if (FindFirstFileA(this->m_path, &data) == INVALID_HANDLE_VALUE)
               return 0;
            return CALL_MEMBER_FN(this, InvalidateAgainstLooseFiles)("Data\\", "", &data.ftLastWriteTime);
         }
   };
   static_assert(sizeof(Archive) >= 0x280, "RE::Archive is too small!");
   static_assert(sizeof(Archive) <= 0x280, "RE::Archive is too large!");
};