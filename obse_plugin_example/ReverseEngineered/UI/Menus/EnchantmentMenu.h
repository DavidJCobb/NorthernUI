#pragma once
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameMenus.h"

namespace RE {
   class EnchantmentMenu : public Menu { // sizeof == 0xA0
      public:
         EnchantmentMenu();
         ~EnchantmentMenu();

         struct Unk0 {
            void*    unk00;
            UInt32   unk01;
            TESForm* form;
         };
         enum {							// these are specified by <id> tags in xml
                                    // passed to SetField to initialize component tiles, etc
            kField_EnchantName = 0x02,
            kField_UsesIcon,
            kField_SoulCostIcon,
            kField_GoldValue,
            kField_KnownEffectPane,
            kField_AddedEffectPane,
            kField_FocusBox,
            kField_KnownEffectScrollbar,
            kField_KnownEffectScrollmarker,
            kField_AddedEffectScrollmarker,
            kField_AddedEffectScrollbar,
            kField_PlayerGoldValue,
            kField_CreateButton,
            kField_ExitButton,
            kField_KnownEffectsText,			// x10
            kField_AddedEffectsText,
            kField_EnchItemRect = 0x14,
            kField_SoulGemRect = 0x16,
            kField_EnchItemIcon = 0x19,
            kField_SoulGemIcon = 0x1A,
         };

         EnchantmentItem*	enchantItem;      // 28 // temp! not the item player gets when enchantment is finished
         Unk0*      soulGemInfo;             // 2C
         Unk0*      enchantableInfo;         // 30 // enchantableInfo->form == unenchanted item
         void*      unk34;                   // 34
         UInt32     cost;                    // 38
         TileText*  enchantNameTile;         // 3C
         TileRect*  nameBackground;          // 40
         TileImage* usesIcon;                // 44
         TileText*  goldValue;               // 48
         TileText*  playerGoldValue;         // 4C
         TileImage* soulCostIcon;            // 50
         TileRect*  knownEffectPane;         // 54
         TileRect*  addedEffectPane;         // 58
         TileRect*  focusBox;                // 5C
         TileImage* knownEffectScrollBar;    // 60
         TileImage* knownEffectScrollMarker; // 64
         TileImage* addedEffectScrollBar;    // 68
         TileImage* addedEffectScrollMarker; // 6C
         TileImage* createButton;            // 70
         TileImage* exitButton;              // 74
         TileText*  knownEffectsText;        // 78
         TileText*  addedEffectsText;        // 7C
         TileRect*  enchItemRect;            // 80
         TileRect*  soulgemRect;             // 84
         TileImage* soulgemIcon;             // 88
         TileImage* enchItemIcon;            // 8C
         void*      unk90;                   // 90 // SKSE: pointer to some struct with EffectSetting info
         TileRect*  unk94;                   // 94 // active tile?
         MenuTextInputState* unk98;          // 98
         UInt8      unk9C = 1;
         UInt8      unk9D = 0;
         UInt8      pad9E;
         UInt8      pad9F;
   };
   static_assert(sizeof(EnchantmentMenu) <= 0x0A0, "RE::EnchantmentMenu is too large!");
   static_assert(sizeof(EnchantmentMenu) >= 0x0A0, "RE::EnchantmentMenu is too small!");
};