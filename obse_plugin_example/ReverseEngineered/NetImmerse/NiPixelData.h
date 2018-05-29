#pragma once
#include "ReverseEngineered/_BASE.h"
#include "obse/NiNodes.h"

class RE {
   class NiPixelData : public NiObject { // sizeof == 0x70
      public:
         NiPixelData();
         ~NiPixelData();

         // face size = unk05C[mipmapLevels]
         // total size = face size * numFaces

         TextureFormat	format;		// 008
         NiRefObject*  unk04C;	// 04C
         UInt32	unk050;			// 050
         UInt32*	width;    // 054 // array; count == mipmapLevels
         UInt32*	height;   // 058 // array; count == mipmapLevels
         UInt32*  unk05C;		// 05C // array; count == mipmapLevels // pixel count offsets? unk5C[x] == (width[x] * height[x]) + unk5C[x - 1]
         UInt32	mipmapLevels;	// 060
         UInt32	unk064;			// 064
         UInt32	unk068;			// 068
         UInt32	numFaces;		// 06C
   };
};