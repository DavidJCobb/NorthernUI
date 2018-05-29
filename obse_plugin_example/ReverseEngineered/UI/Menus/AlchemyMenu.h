#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameExtraData.h"
#include "obse/GameMenus.h"

namespace RE {
   extern UInt32* const AlchemyMenuClickedIngredient; // read by RepairMenu when handling an ingredient selection

   class AlchemyMenu : public Menu { // sizeof == 0xC0
      public:
		   enum { kID = 0x410 };
         AlchemyMenu();
         ~AlchemyMenu();

         struct EffectData {
            UInt32 effectCode;
            UInt32 unk04;      // magnitude?
         };
         struct EffectEntry {
            EffectData*  data;
            EffectEntry* next;
         };

         enum UnkA4 : UInt8 {
            kValue_Empty = 0, // the name is empty or invalid and should be cleared
               // - possible entrances:
               //    - set by UpdateTextField if the text input has no value and the potion is nameless
               //    - set by UpdatePotion after resetting the potion's name (only done if value wasn't previously 2)
               // - effects:
               //    - clicking into the potion name textbox will empty it out
               //    - prevents UpdateTextField from doing anything
               // - possible exits:
               //    - [general]  set to 2 when HandleKeyboardInput runs, if the textbox has focus
               //    - [specific] set to 1 when UpdatePotion renames the potion based on its first effect
               //    - [general]  set to 3 when a potion is cooked
            kValue_Auto = 1, // the name is being handled automatically
               // - possible entrances:
               //    - set by HandleSelectedIngredient if Arg1 is truthy and the value was previously 3
               //    - set by UpdatePotion if it updates a non-empty potion's name
               // - effects:
               //    - no direct effects; the value is never checked for
               //    - consequences of it being set:
               //       - [counters 0] the textbox won't be cleared when focused
               //       - [counters 0] UpdateTextField will run
               //       - [counters 2] UpdatePotion can rename the potion
               //       - [counters 3] UpdatePotion can rename the potion
               // - possible exits:
               //    - [general]  UpdateTextField sets A4 to 0 if the text input has no value and the potion is nameless
               //    - [general]  set to 0 when UpdatePotion resets the name of a potion with no effects (only if it's not 2)
               //    - [specific] set to 0 when UpdatePotion resets the name of a potion with no effects
               //    - [general]  set to 3 when a potion is cooked
            kValue_Custom = 2, // the name has been customized by the player (or we just assume it has been)
               // - possible entrances:
               //    - set by HandleKeyboardInput if the textbox has focus (even if the key input is for navigation, e.g. moving the text caret with the arrow keys)
               // - effects:
               //    - prevents UpdatePotion from renaming the potion when effects are updated
               //    - prevents UpdatePotion from renaming the potion when effects fail to update (i.e. empty potion)
               // - possible exits:
               //    - [general] UpdateTextField sets A4 to 0 if the text input has no value and the potion is nameless
               //    - [general] set to 3 when a potion is cooked
            kValue_NewPotion = 3, // a new potion has been crafted; if ingredients change, revert us to Auto
               // - possible entrances:
               //    - set after cooking a potion
               // - effects:
               //    - prevents UpdatePotion from renaming the potion when effects are updated
               // - possible exits:
               //    - [general]  UpdateTextField sets A4 to 0 if the text input has no value and the potion is nameless
               //    - [specific] set to 1 by HandleSelectedIngredient if Arg1 is truthy
               //    - [general]  set to 0 when UpdatePotion resets the name of a potion with no effects (only if it's not 2)
         };

         TileRect*  nameBackground;   // 28
         TileText*  nameText;         // 2C
         TileImage* appaIcons[4];     // 30
         TileRect*  ingredRects[4];   // 40
         TileRect*  effectList;       // 50
         TileRect*  focusBox;         // 54
         TileImage* createButton;     // 58
         TileImage* exitButton;       // 5C
         TileImage* scrollBar;        // 60
         TileImage* scrollMarker;     // 64
         TileImage* ingredIcons[4];   // 68
         ExtraContainerChanges::EntryData* apparatus[4]; // 78
         float        luckModifiedSkill; // 88 // player's Alchemy skill, factoring in luck, as of the last call to UpdatePotion
         UInt32       unk8C;          // 8C
         Tile*        focusBoxTarget; // 90
         AlchemyItem* potion;         // 94 // current working potion: we modify this in order to handle effects, etc.
         float        potionWeight;   // 98 // == average weight of all selected ingredients; updated in HandleSelectedIngredient
         UInt32       selectedIngredientCount; // 9C // == number of valid ingredients, updated when one is selected and processed
         MenuTextInputState* potionNameInputState; // A0
         UInt8        unkA4; // A4 // enum; values seen include 0, 1, 2, and 3; 2 and 3 both indicate that a custom name was entered and should be kept when the potion is updated; otherwise, if the potion's name is reset, this field is set to 1
         UInt8        unkA5; // A5 // not initialized
         UInt8        unkA6; // A6 // init'd to 0xFF
         UInt8        unkA7;
         EffectEntry  effects; // A8
         ExtraContainerChanges::EntryData* ingreds[4]; // B0

         IngredientItem* GetIngredientItem(UInt32 whichIngred);
         TESObjectAPPA*  GetApparatus(UInt32 whichAppa);
         UInt32          GetIngredientCount(UInt32 whichIngred);

         MEMBER_FN_PREFIX(AlchemyMenu);
         DEFINE_MEMBER_FN(CookPotion,               void, 0x00594CA0); // doesn't verify that the potion has any effects before cooking
         DEFINE_MEMBER_FN(HandleSelectedIngredient, void, 0x00594F00, bool update); // updates UI and potion state based on the value of AlchemyMenuClickedIngredient; the caller should already have directly set AlchemyMenu::ingreds[*AlchemyMenuClickedIngredient]
         DEFINE_MEMBER_FN(RenderPotionEffect,       void, 0x00593B20, EffectItem*, UInt32 listindex); // Called by UpdatePotion; listed here for documentation purposes; do not call it yourself
         DEFINE_MEMBER_FN(UpdatePotion,             void, 0x00593CD0); // update the current working potion based on all selected ingredients
         DEFINE_MEMBER_FN(UpdateTextField,          void, 0x00593710); // update the text displayed in the name textbox to match internal state
   };
   static_assert(sizeof(AlchemyMenu) <= 0xC0, "RE::AlchemyMenu is too large!");
   static_assert(sizeof(AlchemyMenu) >= 0xC0, "RE::AlchemyMenu is too small!");
   STATIC_ASSERT(offsetof(AlchemyMenu, effects) == 0x0A8);

   DEFINE_SUBROUTINE_EXTERN(void,  CloseAlchemyMenu, 0x005932B0); // TODO: compare to a similar subroutine that exists for LockPickMenu
   DEFINE_SUBROUTINE_EXTERN(UInt8, AlchemySkillLevelToRevealedEffectCount, 0x0041BA00, UInt32 skill);
   DEFINE_SUBROUTINE_EXTERN(float, ApplyMortarAndPestleMult, 0x00548400, float mortalAndPestleQuality, float playerLuckModifiedSkill); // "influence" for the effect strength funcs
   //
   // These next funcs use, as their args:
   //  - Out var(s)
   //  - Float: EffectSetting base cost
   //  - Float: Result of a ApplyMortarAndPesleResult call
   //  - UInt8: Unknown enum:
   //
   //                  |  EFFECT  | APPARATUS 1 | APPARATUS 2 | APPARATUS 3
   //           Result | Hostile? |   Alembic?  | Calcinator? |   Retort?     // according to OBSE documentation
   //    --------------+----------+-------------+-------------+------------
   //                1 |       No |   Either/Or |        None |      Exists
   //                4 |       No |   Either/Or |      Exists |        None
   //    Previous or 0 |      Yes |        None |        None |   Either/Or
   //                3 |      Yes |        None |      Exists |   Either/Or
   //                2 |      Yes |      Exists |        None |   Either/Or
   //                5 |      Yes |      Exists |      Exists |   Either/Or
   //
   //  - Bool:  Whether the effect is hostile (use EffectItem::IsHostile)
   //  - Float: Quality of apparatus[3]
   //  - Float: Quality of apparatus[1]
   //  - Float: Quality of apparatus[2]
   //  - Bool:  Whether the effect's containing list has any non-hostile effects (use EffectItemList::HasNonPoisonousEffects). If false, result is a poison.
   //
   DEFINE_SUBROUTINE_EXTERN(void,  ComputeType1PotionEffectStrength, 0x00548420, float* outMagnitude, float* outDuration, float baseCost, float mortarAndPestleInfluence, UInt32, bool isHostile, float, float, float, bool isNotPoison);
   DEFINE_SUBROUTINE_EXTERN(void,  ComputeType2PotionEffectStrength, 0x005486A0, float* outDuration,                      float baseCost, float mortarAndPestleInfluence, UInt32, bool isHostile, float, float, float, bool isNotPoison);
   DEFINE_SUBROUTINE_EXTERN(void,  ComputeType3PotionEffectStrength, 0x005487F0, float* outMagnitude,                     float baseCost, float mortarAndPestleInfluence, UInt32, bool isHostile, float, float, float, bool isNotPoison);
};