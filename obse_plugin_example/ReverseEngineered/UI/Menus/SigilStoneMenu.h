#pragma once
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameMenus.h"

namespace RE {
   class SigilStoneMenu : public Menu { // sizeof == 0x80
      public:
         SigilStoneMenu();
         ~SigilStoneMenu();

         UInt32 unk28 = 0;
         UInt32 unk2C = 0;
         UInt32 unk30 = 0;
         UInt32 unk34 = 0;
         UInt32 unk38 = 0;
         UInt32 unk3C = 0;
         UInt32 unk40 = 0;
         UInt32 unk44 = 0;
         UInt32 unk48 = 0;
         UInt32 unk4C = 0;
         UInt32 unk50 = 0;
         UInt32 unk54 = 0;
         UInt32 unk58 = 0;
         UInt32 unk5C = 0;
         UInt32 unk60 = 0;
         UInt32 unk64 = 0;
         UInt32 unk68 = 0;
         UInt32 unk6C = 0;
         UInt32 unk70 = 0;
         MenuTextInputState* unk74;
         UInt8  unk78 = 1;
         UInt8  unk79[3];
         UInt32 unk7C = 0;
   };
   static_assert(sizeof(SigilStoneMenu) <= 0x80, "RE::SigilStoneMenu is too large!");
   static_assert(sizeof(SigilStoneMenu) >= 0x80, "RE::SigilStoneMenu is too small!");
};
