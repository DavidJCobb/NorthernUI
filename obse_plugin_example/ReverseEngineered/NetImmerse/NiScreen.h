#pragma once
#include "NiProperty.h"
#include "obse/NiObjects.h"

namespace RE {
   class NiScreenPolygon : NiObject { // sizeof == 0x1C
      public:
         //
         // No additional virtual methods.

         NiPropertyState* unk08; // 08
         UInt16 count; // 0C // count for all three arrays; vertex count?
         UInt16 pad0E;
         void*  unk10; // 10 // variably sized array; sizeof(item) == 0xC
         void*  unk14; // 14 // variably sized array; can be nullptr; sizeof(item) == 0x8
         NiColorAlpha* unk18; // 18 // variably sized array; can be nullptr
   };
};