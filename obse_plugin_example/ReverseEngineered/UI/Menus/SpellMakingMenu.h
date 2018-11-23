#pragma once
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameMenus.h"

namespace RE {
   class SpellMakingMenu : public Menu { // sizeof == 0x78
      public:
         SpellMakingMenu();
         ~SpellMakingMenu();

         UInt32 unk28 = 0;
         UInt32 unk2C = 0;
         UInt32 unk30 = 0;
         Tile*  unk34 = nullptr; // Tile* or subclass
         Tile*  scrollbarKnownEffects = nullptr; // 38
         Tile*  unk3C = nullptr; // Tile* or subclass // most likely known-effects scroll thumb
         Tile*  scrollbarAddedEffects = nullptr; // 40
         Tile*  unk44 = nullptr; // Tile* or subclass // most likely added-effects scroll thumb
         UInt32 unk48 = 0;
         UInt32 unk4C = 0;
         UInt32 unk50 = 0;
         TileText* unk54 = nullptr;
         UInt32 unk58 = 0;
         UInt32 unk5C = 0;
         UInt8  unk60 = 0xFF;
         UInt8  unk61[3];
         float  unk64 = 0.0;
         float  unk68 = 0.0;
         UInt8  unk6C = 0; // analogous to AlchemyMenu::unkA4
         UInt8  unk6D[3];
         MenuTextInputState* unk70;
         void*  unk74 = nullptr;

         MEMBER_FN_PREFIX(SpellMakingMenu);
         DEFINE_MEMBER_FN(FocusTextField, void, 0x005D7590);
   };
   static_assert(sizeof(SpellMakingMenu) <= 0x78, "RE::SpellMakingMenu is too large!");
   static_assert(sizeof(SpellMakingMenu) >= 0x78, "RE::SpellMakingMenu is too small!");
};