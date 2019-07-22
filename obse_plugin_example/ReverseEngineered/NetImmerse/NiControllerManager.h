#pragma once
#include "obse/NiControllers.h"

namespace RE {
   // 080
   class NiControllerManager : public NiTimeController { // sizeof == 0x80
      public:
         NiControllerManager();
         ~NiControllerManager();

         NiTArray<NiControllerSequence*> sequences; // 03C - NiTArray <NiPointer <NiControllerSequence> > >
         NiTArray<NiControllerSequence*> unk04C; // 4C
         NiTStringPointerMap <NiControllerSequence *>	sequenceMap;	// 058
         UInt32	pad06C;	// 06C
         UInt32	pad070;	// 070
         UInt32	pad074;	// 074
         UInt32	pad078;	// 078
         NiDefaultAVObjectPalette* objectPalette;	// 07C
   };
};
