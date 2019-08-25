#pragma once
#include "BSFile.h"
#include "shared.h"
#include "ReverseEngineered/_BASE.h"

namespace RE {
   enum BSAFlags : UInt32 {
      kBSAFlag_HasFolderNames  = 0x0001,
      kBSAFlag_HasFileNames    = 0x0002,
      kBSAFlag_Compressed      = 0x0004,
      kBSAFlag_Unk0008         = 0x0008, // related to retaining directory strings/offsets
      kBSAFlag_RetainFilenameStrings = 0x0010,
      kBSAFlag_RetainFilenameOffsets = 0x0020,
      kBSAFlag_IsXbox360Archive      = 0x0040,
      kBSAFlag_Unk0080         = 0x0080, // related to retaining directory strings/offsets
      kBSAFlag_Unk0100         = 0x0100,
      kBSAFlag_Unk0200         = 0x0200,
      kBSAFlag_Unk0400         = 0x0400,
   };
   enum BSAFiletype : UInt32 {
      kBSAFiletype_Meshes   = 0,
      kBSAFiletype_Textures = 1,
      kBSAFiletype_Menus    = 2,
      kBSAFiletype_Sounds   = 3,
      kBSAFiletype_Voices   = 4,
      kBSAFiletype_Shaders  = 5,
      kBSAFiletype_Trees    = 6,
      kBSAFiletype_Fonts    = 7,
      kBSAFiletype_Misc     = 8,
      //
      kBSAFiletype_COUNT    = 9,
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

   struct BSHash {
      UInt64 data;

      MEMBER_FN_PREFIX(BSHash);
      DEFINE_MEMBER_FN(Constructor,        BSHash&, 0x006FA2D0, const char* str, UInt32 type);
      DEFINE_MEMBER_FN(Subroutine0042BC10, UInt32,  0x0042BC10, const BSHash& other);
   };
   DEFINE_SUBROUTINE_EXTERN(void, HashFilePath, 0x006FA1B0, const char* filename, BSHash& outFolder, BSHash& outFile);

   class Archive;
   class ArchiveFile;

   constexpr UInt32 ce_bsaSignatureBSwapped = '\0ASB';

   extern LinkedPointerList<Archive>* const g_archiveList; // All loaded BSAs in this order: INI-specified archives; non-Oblivion.esm archives in the same order as the load order. Oblivion BSAs are specified by the INI (though Mod Organizer can override this in a way that looks confusing).
   extern Archive** const g_archiveWhichProvidedLastFile; // speeds up lookups in FindBSAThatContainsFile; SkyBSA works in part by deliberately preventing this pointer from ever being used

   // Used by:
   //  - LazyFileLookup (bottom of this file)
   //  - sub0042EBC0
   //
   extern Archive** const g_firstLoadedArchivesByType;  // g_firstLoadedArchivesByType[type] == first loaded Archive flagged with that filetype
   extern Archive** const g_secondaryArchiveByTypeList; // offset from the first list by sizeof(void*) * 9

   //
   // These maps are used for ArchiveInvalidation.txt and ONLY for ArchiveInvalidation.txt:
   //
   extern NiTArray<BSHash>* const g_archiveInvalidatedFilenames;
   extern NiTArray<BSHash>* const g_archiveInvalidatedDirectoryPaths;

   DEFINE_SUBROUTINE_EXTERN(BSAFiletypeFlags, FileExtensionToBSAFiletypeFlags, 0x0042BA90, const char*);
   DEFINE_SUBROUTINE_EXTERN(void, FindBSAThatContainsFile, 0x0042EA60, const char* filepath, UInt32 filetypeFlags);
   DEFINE_SUBROUTINE_EXTERN(ArchiveFile*, FindFileInBSA, 0x0042E8E0, const char* filepath, UInt32, UInt32 filetypeFlags);
   DEFINE_SUBROUTINE_EXTERN(void, InvalidateFileInAllLoadedBSAs, 0x0042EDF0, const BSHash& folder, const BSHash& file, UInt16 filetypeFlags); // only affects BSAs in g_archiveList, so Archives can use it while loading; in practice, none should

   // Not sure if LoadBSAFile is safe to call; others almost certainly aren't, or are already called during startup
   DEFINE_SUBROUTINE_EXTERN(void, LoadBSAFile, 0x0042F4C0, const char* filePath, UInt16 overrideFiletypeFlags, UInt32 zero); // adds archive to g_archiveList
   DEFINE_SUBROUTINE_EXTERN(void, DiscardAllBSARetainedFilenames, 0x0042C970);
   DEFINE_SUBROUTINE_EXTERN(void, ReadArchiveInvalidationTxtFile, 0x0042D840, const char* filename);
   DEFINE_SUBROUTINE_EXTERN(void, RemoveFromTypeToBSAMap, 0x0042BE10, Archive*); // called when an Archive is deleted

   struct BSAEntry { // "File Record" or "Folder Record" in UESP docs
      BSHash hash; // 00 // hash of the file/folder path
      union {
         UInt32 count = 0; // number of files
         UInt32 size;      // size of the file in bytes
      }; // 08
      union {
         UInt32    offset = 0; // data offset within the file (where it's an offset FROM differs between folders and files)
         BSAEntry* files;      // array of file entries for this folder, once the folder is loaded (happens in Archive constructor)
      }; // 0C

      MEMBER_FN_PREFIX(BSAEntry);
      DEFINE_MEMBER_FN(Constructor, BSAEntry&, 0x0042BD20);

      UInt32 getOffset() const { // files only
         return this->offset & 0x7FFFFFFF;
      }
      UInt32 getSize() const { // files only
         return this->size & 0x3FFFFFFF;
      }
      bool isNonDefaultCompression() const { // files only
         return (this->size & 0x40000000);
      }
      bool isKnownNotToBeOverridden() const { // files only
         return (this->size & 0x80000000); // set by Archive::CheckFileIsOverridden once we know the file isn't overridden by a loose file
      }
      bool isInvalidated() const { // files only
         return (this->offset & 0x80000000) == 0;
      }
      void invalidate() { // files only
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

   class ArchiveFile : public BSFile { // sizeof == 0x15C
      public:
         Archive* owner;  // 154
         UInt32   offset; // 158 // offset within the BSA file

         MEMBER_FN_PREFIX(ArchiveFile);
         DEFINE_MEMBER_FN(Constructor, ArchiveFile&, 0x0042D540, const char* path, Archive* owner, UInt32 offset, UInt32 filesize, SInt32);
   };
   class CompressedArchiveFile : public ArchiveFile { // sizeof == 0x174
      public:
         enum { kVTBL = 0x00A35E64 };

         // TODO

         MEMBER_FN_PREFIX(CompressedArchiveFile);
         DEFINE_MEMBER_FN(Constructor, CompressedArchiveFile&, 0x0042D6D0, UInt32, UInt32, UInt32, UInt32, UInt32);
   };

   class Archive : public BSFile { // sizeof == 0x280
      public:
         enum Flags194 {
            kFlag194_Unk08 = 0x08,
         };
         
         BSAHeader header; // 154
         BSAEntry* folders = nullptr; // 178 // array
         UInt32 unk17C;
         __time64_t myDateModified; // 180 // same type as the Date Modified in stat()
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
         UInt32 refCount = 0; // 1A8
         bool   queuedForDeletion = 0; // 1AC
         UInt8  unk1AD;
         UInt8  unk1AE;
         UInt8  unk1AF;
         UInt32 unk1B0;
         UInt32 unk1B4[(0x200 - 0x1B4) / 4];
         CRITICAL_SECTION unk200; // 200 // wrapped in a struct that allows for automatic initialization/destruction
         UInt32 unk218[(0x280 - 0x218) / 4];

         /*// Functions provided as example documentation only. At a minimum they need nullptr checks.
         //
         const char* getFileName(UInt8 folderIndex, UInt8 fileIndex) const {
            auto offset = this->fileNameOffsetsByFolder[folderIndex][fileIndex];
            return &this->fileNames[offset];
         }
         const char* getFolderName(UInt8 index) const {
            auto offset = this->folderNameOffsets[index];
            return &this->folderNames[offset];
         }
         //*/

         MEMBER_FN_PREFIX(Archive);
         DEFINE_MEMBER_FN(Constructor, Archive&, 0x0042EE80, const char* filePath, UInt32, bool, UInt32);
         DEFINE_MEMBER_FN(CheckFileIsOverridden, bool, 0x0042C1D0, BSAEntry& file, const char* looseFilePath); // called by FolderContainsFile; invalidates the file if it's older than a matching loose file
         DEFINE_MEMBER_FN(ContainsFile,   bool, 0x0042E020, const BSHash& file, const BSHash& folder, UInt32& outFolderIndex, UInt32& outFileIndexInFolder, const char* normalizedFilepath); // just calls ContainsFolder and FolderContainsFile
         DEFINE_MEMBER_FN(ContainsFolder, bool, 0x0042CE40, const BSHash& folder, UInt32& outFolderIndex, const char* normalizedFilepath);
         DEFINE_MEMBER_FN(DiscardRetainedFilenames,   void, 0x0042C0D0, UInt32); // conditional on unk194 flag 0x04
         DEFINE_MEMBER_FN(FolderContainsFile, bool, 0x0042D000, UInt32 folderIndex, const BSHash& file, UInt32& outFileIndexInFolder, const char* normalizedFilepath, UInt32 zero); // file path is used for CheckFileIsOverridden
         DEFINE_MEMBER_FN(GetFileByEntry,   ArchiveFile*, 0x0042E1A0, const BSAEntry& file, SInt32, const char* normalizedFilepath); // only used for lazy lookups
         DEFINE_MEMBER_FN(GetFileByIndices, ArchiveFile*, 0x0042E070, UInt32 folderIndex, UInt32 fileIndex, SInt32, const char* normalizedFilepath);
         DEFINE_MEMBER_FN(GetFileEntry, BSAEntry*,    0x0042D240, const BSHash& folder, const BSHash& file, const char* normalizedFilepath); // only used by LazyFileLookup
         DEFINE_MEMBER_FN(RetainsFilenameStringTable, bool, 0x0042BD30);
         DEFINE_MEMBER_FN(RetainsFilenameOffsetTable, bool, 0x0042BD50);
         DEFINE_MEMBER_FN(Subroutine0042BD70, bool, 0x0042BD70);
         //
         DEFINE_MEMBER_FN(CheckDelete, void, 0x0042C910); // 
         DEFINE_MEMBER_FN(DecRef,      void, 0x0042C910); // Bethesda calls this "CheckDelete;" define it with both names
         //
         // The below are all called during the constructor, so don't use them:
         //
         DEFINE_MEMBER_FN(InvalidateOlderFiles, UInt32, 0x0042E2D0); // returns number of files invalidated
         DEFINE_MEMBER_FN(InvalidateAgainstLooseFiles, UInt32, 0x0042D2A0, const char* pathRoot, const char* pathDeep, FILETIME& myLastModDate); // called by InvalidateOlderFiles
         DEFINE_MEMBER_FN(LoadFolderNames,      const char*, 0x0042CAE0, UInt32 stopAt); // returns loaded folder names
         DEFINE_MEMBER_FN(SetReadWriteFuncs,    void,   0x004307F0, bool);

         /*// Function provided as example documentation only.
         //
         inline UInt32 _example_InvalidateAgainstLooseFiles() {
            WIN32_FIND_DATAA data;
            if (FindFirstFileA(this->m_path, &data) == INVALID_HANDLE_VALUE)
               return 0;
            return CALL_MEMBER_FN(this, InvalidateAgainstLooseFiles)("Data\\", "", data.ftLastWriteTime);
         }
         //*/

         inline void IncRef() {
            InterlockedIncrement(&this->refCount);
         };
   };
   static_assert(sizeof(Archive) >= 0x280, "RE::Archive is too small!");
   static_assert(sizeof(Archive) <= 0x280, "RE::Archive is too large!");

   //
   // QueuedFileEntry relies on LazyFileLookup to get a BSAEntry pointer, which gets 
   // stored to QueuedFileEntry::unk24. I'm not sure where that actually gets used, but 
   // presumably QueuedFileEntry has no way to refer to any BSA other than the ones in 
   // g_firstLoadedArchivesByType.
   //
   // The real object of interest, however, is QueuedTexture::Unk_01. This function 
   // checks its unk24 pointer -- also a BSAEntry -- and retrieves the actual stored 
   // file from the BSA, i.e. an ArchiveFile*. And as it turns out, QueuedTexture also 
   // uses LazyFileLookup to store a BSAEntry pointer to its unk24.
   //
   // The purpose of BSA Redirection, then, is to ensure that these lookups always fail, 
   // because the redirect BSA is g_firstLoadedArchivesByType[kBSAFiletypeFlag_Textures] 
   // and it contains only a dummy file that will never match anything. These lookups 
   // only check that BSA, so if they fail, then what do their callers do? What calls 
   // QueuedTexture::Unk_01?
   //
   DEFINE_SUBROUTINE_EXTERN(BSAEntry*, LazyFileLookup, 0x0042DB10, BSAFiletype filetype, const BSHash& folder, const BSHash& file, const char* normalizedFilepath);
};