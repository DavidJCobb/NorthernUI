#include "Archive.h"

namespace RE {
   LinkedPointerList<Archive>* const g_archiveList = (LinkedPointerList<Archive>*) 0x00B338E0;
   Archive** const g_archiveWhichProvidedLastFile = (Archive**) 0x00B338E4;
   Archive** const g_firstLoadedArchivesByType    = (Archive**)0x00B338E8;
   NiTArray<BSHash>* const g_archiveInvalidatedFilenames = (NiTArray<BSHash>*) 0x00B33930;
   NiTArray<BSHash>* const g_archiveInvalidatedDirectoryPaths = (NiTArray<BSHash>*) 0x00B33934;

   DEFINE_SUBROUTINE(void, HashFilePath, 0x006FA1B0, const char* filename, BSHash& outFile, BSHash& outFolder);

   DEFINE_SUBROUTINE(void, FindBSAThatContainsFile, 0x0042EA60, const char* filepath, UInt32 filetypeFlags);
   DEFINE_SUBROUTINE(void, InvalidateFileInAllLoadedBSAs, 0x0042EDF0, const BSHash& folder, const BSHash& file, UInt16 filetypeFlags);

   DEFINE_SUBROUTINE(void, LoadBSAFile, 0x0042F4C0, const char* filePath, UInt16 overrideFiletypeFlags, UInt32 zero); // adds archive to g_archiveList
   DEFINE_SUBROUTINE(void, DiscardAllBSARetainedFilenames, 0x0042C970);
   DEFINE_SUBROUTINE(void, ReadArchiveInvalidationTxtFile, 0x0042D840, const char* filename);
};