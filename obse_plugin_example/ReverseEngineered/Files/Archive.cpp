#include "Archive.h"

namespace RE {
   LinkedPointerList<Archive>* const g_archiveList = (LinkedPointerList<Archive>*) 0x00B338E0;
   NiTArray<BSHash>* const g_archiveInvalidatedFilenames = (NiTArray<BSHash>*) 0x00B33930;
   NiTArray<BSHash>* const g_archiveInvalidatedDirectoryPaths = (NiTArray<BSHash>*) 0x00B33934;

   DEFINE_SUBROUTINE(void, HashFilePath, 0x006FA1B0, const char* filename, BSHash& outFile, BSHash& outFolder);

   DEFINE_SUBROUTINE(void, LoadBSAFile, 0x0042F4C0, const char* filePath, UInt32 zeroA, UInt32 zeroB); // adds archive to g_archiveList
   DEFINE_SUBROUTINE(void, DiscardAllBSARetainedFilenames, 0x0042C970);
   DEFINE_SUBROUTINE(void, ReadArchiveInvalidationTxtFile, 0x0042D840, const char* filename);
};