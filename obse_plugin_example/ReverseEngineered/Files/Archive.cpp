#include "Archive.h"

namespace RE {
   LinkedPointerList<Archive>* const g_archiveList = (LinkedPointerList<Archive>*) 0x00B338E0;
   NiTArray<BSHash>* const g_archiveInvalidatedFilenames = (NiTArray<BSHash>*) 0x00B33930;
   NiTArray<BSHash>* const g_archiveInvalidatedDirectoryPaths = (NiTArray<BSHash>*) 0x00B33934;
};