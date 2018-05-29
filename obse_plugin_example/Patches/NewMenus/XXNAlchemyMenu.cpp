#include "XXNAlchemyMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/AlchemyMenu.h"

XXNAlchemyMenu::ItemAndIndex::ItemAndIndex(RE::ExtraContainerChanges::EntryData* data, UInt32 index) {
   this->data  = data;
   this->index = index;
   this->name  = CALL_MEMBER_FN(data, GetName)();
};
XXNAlchemyMenu::ItemAndIndex::~ItemAndIndex() {
   if (this->data) {
      CALL_MEMBER_FN(this->data, Destructor)();
      FormHeap_Free(this->data);
      this->data = nullptr;
   }
};
bool XXNAlchemyMenu::ItemAndIndex::operator<(const XXNAlchemyMenu::ItemAndIndex& rhs) const {
   return (_stricmp(this->name, rhs.name) < 0);
};

XXNAlchemyMenu* XXNAlchemyMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNAlchemyMenu));
   if (memory) {
      auto result = new (memory) XXNAlchemyMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNAlchemyMenu::XXNAlchemyMenu() {
   this->potion = RE::AlchemyItem::Create();
   this->textInputState = RE::MenuTextInputState::Create();
};
XXNAlchemyMenu::~XXNAlchemyMenu() {
   if (this->potion) {
      //
      // TODO: Does AlchemyMenu do anything extra with this?
      //
      this->potion->Destroy(true); // OBSE has the bool backwards; it's actually a "do dealloc" bool
      this->potion = nullptr;
   }
   if (this->textInputState) {
      CALL_MEMBER_FN(this->textInputState, Destructor)();
      FormHeap_Free(this->textInputState);
      this->textInputState = nullptr;
   }
   this->ClearInventory();
   {  // Clear apparatus references (per vanilla CloseAlchemyMenu subroutine)
      auto& list = this->apparati;
      UInt8 i = 0;
      do {
         if (!list[i])
            continue;
         CALL_MEMBER_FN(list[i], Destructor)();
         FormHeap_Free(list[i]);
         list[i] = nullptr;
      } while (++i < std::extent<decltype(XXNAlchemyMenu::apparati)>::value);
   }
};

void XXNAlchemyMenu::Dispose(bool free) {
   this->~XXNAlchemyMenu();
   if (free)
      FormHeap_Free(this);
};

void XXNAlchemyMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
   if (newID >= kTileID_Apparatus1 && newID <= kTileID_Apparatus4) {
      newID -= kTileID_Apparatus1;
      this->tileApparati[newID] = tile;
      return;
   } else if (newID == kTileID_Background) {
      this->tileBackground = tile;
   } else if (newID == kTileID_ItemStats) {
      this->tileItemStats = tile;
   } else if (newID == kTileID_FocusBox) {
      this->tileFocusBox = tile;
   } else if (newID == kTileID_ButtonCreatePotion) {
      this->tileButtonCreatePotion = tile;
   } else if (newID == kTileID_ButtonDeselectAll) {
      this->tileButtonDeselectAll = tile;
   } else if (newID == kTileID_ButtonExit) {
      this->tileButtonExit = tile;
   } else if (newID == kTileID_EffectListContainer) {
      this->effectList.container = tile;
   } else if (newID == kTileID_EffectListScrollbar) {
      this->effectList.scrollBar = tile;
   } else if (newID == kTileID_EffectListScrollThumb) {
      this->effectList.scrollThumb = tile;
   } else if (newID == kTileID_IngredientListContainer) {
      this->ingredientList.container = tile;
   } else if (newID == kTileID_IngredientListScrollbar) {
      this->ingredientList.scrollBar = tile;
   } else if (newID == kTileID_IngredientListScrollThumb) {
      this->ingredientList.scrollThumb = tile;
   } else if (newID == kTileID_PotionEffectListContainer) {
      this->potionEffectList.container = tile;
   } else if (newID == kTileID_PotionEffectListScrollbar) {
      this->potionEffectList.scrollBar = tile;
   } else if (newID == kTileID_PotionEffectListScrollThumb) {
      this->potionEffectList.scrollThumb = tile;
   } else if (newID == kTileID_NameTextboxBackground) {
      this->tileNameBackground = tile;
   } else if (newID == kTileID_NameTextboxText) {
      this->tileNameText = tile;
   }
};
void XXNAlchemyMenu::HandleMouseUp(SInt32 id, RE::Tile* target) {
   if (this->textInputState->hasFocus) {
      CALL_MEMBER_FN(this->textInputState, SetHasFocus)(false);
      this->UpdateTextField();
   } else if (id == kTileID_NameTextboxBackground || id == kTileID_NameTextboxText) {
      //
      // Focus the textbox.
      //
      if (this->potionNameState == kTextboxState_Empty) {
         CALL_MEMBER_FN(this->tileNameText, UpdateString)(kTileValue_string, "");
      }
      CALL_MEMBER_FN(this->textInputState, SetFontID)(this->tileNameText->GetFontIndex());
      CALL_MEMBER_FN(this->textInputState, SetTextWrapwidth)(CALL_MEMBER_FN(this->tileNameText, GetFloatTraitValue)(kTileValue_wrapwidth));
      CALL_MEMBER_FN(this->textInputState, SetHasFocus)(true);
      this->UpdateTextField();
      return;
   }
   //
   if (id == kTileID_ButtonCreatePotion) {
      if (CALL_MEMBER_FN(target, GetFloatTraitValue)(RE::kButtonTraitValue_Enabled) == 1.0F)
         return;
      this->CreatePotion();
      return;
   } else if (id == kTileID_ButtonDeselectAll) {
      if (CALL_MEMBER_FN(target, GetFloatTraitValue)(RE::kButtonTraitValue_Enabled) == 1.0F)
         return;
      this->DeselectAll();
      return;
   } else if (id == kTileID_ButtonExit) {
      this->Close();
      return;
   } else if (id == kTileID_InventoryShowAllCategories) { // Show all items regardless of category (i.e. don't filter ingredients by effect)
      this->selectedEffect = 0;
      this->UpdateRenderedEffectList();
      this->RenderInventory();
      return;
   } else if (id == kTileID_InventoryCategory) { // Select an item category (i.e. filter ingredients by effect)
      UInt64 effectAndAV = 0;
      {
         UInt32 effectCode;
         UInt32 avOrOther;
         {
            auto code = CALL_MEMBER_FN(target, GetStringTraitValue)(kInventoryCategoryTrait_EffectCode); // the effect code written as a const char[5]
            if (!code)
               return;
            effectCode = *((UInt32*) code); // NOTE: Don't bswap; it's stringified with the right endianness as is
            if (!effectCode)
               return;
         }
         avOrOther = (UInt32) CALL_MEMBER_FN(target, GetFloatTraitValue)(kInventoryCategoryTrait_AVOrOther);
         effectAndAV = effectCode;
         effectAndAV |= ((UInt64) avOrOther) << 32;
      }
      for (auto it = this->availableEffects.begin(); it != this->availableEffects.end(); ++it) {
         if (it->code == effectAndAV) {
            this->selectedEffect = effectAndAV;
            break;
         }
      }
      this->UpdateRenderedEffectList();
      this->RenderInventory();
      return;
   } else if (id == kTileID_InventoryItem) { // Select or deselect an ingredient.
      if (!target)
         return;
      if (CALL_MEMBER_FN(target, GetFloatTraitValue)(kInventoryTrait_ItemIsUsable) == 1.0F)
         //
         // In Oblivion, you cannot make a potion out of two matching pairs of ingredients if those pairs 
         // do not have effects in common; i.e. even if you can make an AB potion and a CD potion, it does 
         // not necessarily follow that you can make an ABCD potion. This is because you can only select 
         // ingredients that have effects in common with already-selected ingredients.
         //
         // In Skyrim, you can only make potions out of three ingredients, not four, which means that the 
         // only way to get two pairs is if they overlap anyway.
         //
         return;
      //
      auto data = this->GetIngredientFromListItem(target);
      if (!data) {
         _MESSAGE("%s: Unable to identify clicked ingredient.", __FUNCTION__);
         return;
      }
      UInt8 available = std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value;
      SInt8 current   = std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value - 1;
      do {
         auto selection = this->selectedIngredients[current];
         if (selection == data) {
            //
            // The clicked ingredient is already selected. Deselect it, update the potion, and 
            // update the rendered inventory to reflect the changed selection.
            //
            if (this->potionNameState == kTextboxState_NewPotion)
               this->potionNameState = kTextboxState_Auto; // UpdatePotion will apply this change
            this->selectedIngredients[current] = nullptr;
            this->UpdatePotion();
            this->UpdateRenderedEffectList();
            this->UpdateRenderedInventory();
            return;
         }
         if (!selection)
            available = current;
      } while (--current >= 0);
      //
      // The clicked ingredient isn't selected. If there is room for more ingredients, then select 
      // it, update the potion, and update the rendered inventory to reflect the changed selection.
      //
      if (available < std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value) {
         if (this->potionNameState == kTextboxState_NewPotion)
            this->potionNameState = kTextboxState_Auto; // UpdatePotion will apply this change
         this->selectedIngredients[available] = data;
         this->UpdatePotion();
         this->UpdateRenderedEffectList();
         this->UpdateRenderedInventory();
      }
//else _MESSAGE("%s: There is no room to select the current ingredient.", __FUNCTION__);
      return;
   }
};
void XXNAlchemyMenu::HandleMouseover(SInt32 tileID, RE::Tile* target) {
   if (!this->tileFocusBox)
      return;
   switch (tileID) {
      case kTileID_InventoryShowAllCategories:
      case kTileID_InventoryCategory:
      case kTileID_InventoryItem:
         Menu::HandleFocusBox(this->tileFocusBox, target);
         return;
   }
   Menu::HandleFocusBox(this->tileFocusBox, nullptr);
};
void XXNAlchemyMenu::HandleMouseout(SInt32 tileID, RE::Tile* target) {
   if (!this->tileFocusBox)
      return;
   Menu::HandleFocusBox(this->tileFocusBox, nullptr);
};
void XXNAlchemyMenu::HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target) {
   if (tileID == -1) { // tile is targetable but has no specific ID
      //
      // The clickable parts of a scrollbar are targetable but use ID -1. They are direct 
      // children of the scrollbar, which has an ID, so if we want to be able to react 
      // properly to the scroll wheel being used while the mouse is over the scrollbar, 
      // then we need to react to elements with ID -1 as follows:
      //
      //  - Get the parent tile.
      //  - Check if the parent tile has an ID.
      //  - If so, react as though the event were fired on that tile.
      //
      if ((target = target->parent) == nullptr)
         return;
      float o;
      if (target->GetFloatValue(kTileValue_id, &o))
         tileID = o;
      else
         return;
   }
   //
   // If list items have IDs, then we must check for those.
   //
   if (tileID == kTileID_InventoryShowAllCategories || tileID == kTileID_InventoryCategory)
      tileID = kTileID_EffectListBackground;
   else if (tileID == kTileID_InventoryItem)
      tileID = kTileID_IngredientListBackground;
   //
   if (tileID >= kTileID_EffectListBackground && tileID <= kTileID_EffectListScrollThumb) {
      Menu::HandleScrollbarMouseWheel(this->effectList.scrollThumb);
      return;
   }
   if (tileID >= kTileID_IngredientListBackground && tileID <= kTileID_IngredientListScrollThumb) {
      Menu::HandleScrollbarMouseWheel(this->ingredientList.scrollThumb);
      return;
   }
   if (tileID >= kTileID_PotionEffectListBackground && tileID <= kTileID_PotionEffectListScrollThumb) {
      Menu::HandleScrollbarMouseWheel(this->potionEffectList.scrollThumb);
      return;
   }
};
void XXNAlchemyMenu::HandleFrame() {
   {  // text box cursor blink
      auto state = this->textInputState;
      if (state->hasFocus) {
         CALL_MEMBER_FN(state, UpdateCursorBlinkState)();
         this->UpdateTextField();
      }
   }
};
bool XXNAlchemyMenu::HandleKeyboardInput(UInt32 inputChar) {
   if (!this->textInputState->hasFocus)
      return false;
   {  // Ignore special keys.
      if (inputChar == (int)'`') // ignore debug console toggle
         return false;
      if (inputChar == (int)'\t') // ignore Tab
         return false;
   }
   CALL_MEMBER_FN(this->textInputState, HandleKeypress)(inputChar);
   char check = inputChar & 0xFF; // inputChar can be negative, and MenuTextInputState expects it to be
   if (check == 0 || check > 6) { // input wasn't an arrow key, home, or end
      this->potionNameState = kTextboxState_Custom;
   }
   this->UpdateTextField();
   return true;
};
UInt32 XXNAlchemyMenu::GetID() {
   return kID;
};

void XXNAlchemyMenu::ClearInventory() {
   {
      auto& map = this->ingredientsByEffect;
      for (auto it = map.begin(); it != map.end(); ++it) {
         auto& list = it->second;
         for (auto it = list.begin(); it != list.end(); ++it) {
            it->data = nullptr;
            it->name = nullptr;
         }
         list.clear();
      }
      map.clear();
   }
   this->ingredients.clear(); // destructors handle freeing memory
   this->availableEffects.clear();
   //
   SInt32 count = std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value - 1;
   for (; count >= 0; --count)
      this->selectedIngredients[count] = nullptr;
   this->activeEffects.clear();
};
void XXNAlchemyMenu::DeselectAll() {
   SInt32 count = std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value - 1;
   for (; count >= 0; --count)
      this->selectedIngredients[count] = nullptr;
   this->UpdatePotion();
   this->UpdateRenderedEffectList();
   this->UpdateRenderedInventory();
};
void XXNAlchemyMenu::UpdateInventory() {
//_MESSAGE("%s: Start", __FUNCTION__);
   this->ClearInventory();
   this->UpdateSkill(); // update known effect count
   //
   auto   player = (RE::PlayerCharacter*) *g_thePlayer; // cast so we can use stuff we've found
   UInt32 count  = CALL_MEMBER_FN((RE::TESObjectREFR*) player, GetInventoryItemCount)(0);
//_MESSAGE("%s: Got inventory item count; it's %d", __FUNCTION__, count);
   UInt32 i = 0;
   auto& list    = this->ingredients;
   list.reserve(count);
   this->availableEffects.reserve(count);
   do {
//_MESSAGE("%s: Looping over player's inventory; iteration %d", __FUNCTION__, i);
      auto data = CALL_MEMBER_FN((RE::TESObjectREFR*) player, GetInventoryItemByIndex)(i, 0);
      if (data) {
         bool destroy = true;
         auto item = data->type;
         if (item && data->countDelta && item->typeID == kFormType_Ingredient && !RE::FormIsGoldAsInCurrency(item) && !item->TIsQuestItem()) {
            if (CALL_MEMBER_FN(data, Subroutine004854F0)(player, false, true, true, false)) {
               destroy = false;
            }
         }
         if (destroy) {
            CALL_MEMBER_FN(data, Destructor)();
            FormHeap_Free(data);
            continue;
         }
      } else
         continue;
//_MESSAGE("%s: Got valid inventory item and didn't disqualify it", __FUNCTION__);
      //
      // If an item of the same type already exists in our list, then merge this in.
      //
      bool  alreadyExisted = false;
      for (auto it = list.begin(); it != list.end(); ++it) {
         auto existing = it->data;
         if (!existing)
            continue;
         if (existing->type == data->type) {
//_MESSAGE("%s: Already have an item of this type; merging", __FUNCTION__);
            existing->countDelta += data->countDelta;
            alreadyExisted = true;
            break;
         }
      }
      if (alreadyExisted) {
         CALL_MEMBER_FN(data, Destructor)();
         FormHeap_Free(data);
         continue;
      }
//_MESSAGE("%s: Adding new item stack to our list", __FUNCTION__);
      //
      // ...Otherwise, add this new item to our list.
      //
      list.emplace(list.end(), data, i);
   } while (++i < count);
//_MESSAGE("%s: Items gathered", __FUNCTION__);
   //
   // Sort the ingredients.
   //
   list.shrink_to_fit();
   std::sort(list.begin(), list.end()); // sort alphabetically
   bool resetSelectedEffect = true;
   {  // Update internal indices on items (see ItemAndIndex definition) and construct category map
      UInt8 knownEffectCount = this->knownEffectCount;
      UInt32 i = 0;
      for (auto it = list.begin(); it != list.end(); ++it, ++i) {
         it->index = i; // update index
         auto  item    = (IngredientItem*) it->data->type;
         auto  effects = (RE::EffectItemList*) &(item->magicItem.list);
         UInt8 i       = 0;
         do {
            auto effect = CALL_MEMBER_FN(effects, GetEffectItemByIndex)(i);
            if (!effect)
               continue;
            EffectCodeAndAV code = XXNAlchemyMenu::GetCombinedIdentifier(effect);
            if (this->selectedEffect == code)
               resetSelectedEffect = false;
            {  // Add the ingredient's known effects to the list of all available effects.
               auto& stored = this->availableEffects;
               bool  found  = false;
               for (auto it = stored.begin(); it != stored.end(); ++it) {
                  if (it->code == code) {
                     found = true;
                     break;
                  }
               }
               if (!found) {
                  char name[100];
                  CALL_MEMBER_FN(effect, GetName)(name);
                  stored.emplace(stored.end(), code, name);
               }
            }
            auto& category = this->ingredientsByEffect[code];
            category.emplace(category.end(), it->data, it->index);
         } while (++i < knownEffectCount);
      }
//_MESSAGE("%s: Category map constructed", __FUNCTION__);
      this->availableEffects.shrink_to_fit();
//_MESSAGE("%s: shrunk available effects", __FUNCTION__);
      std::sort(this->availableEffects.begin(), this->availableEffects.end());
      if (resetSelectedEffect)
         this->selectedEffect = 0;
      {  // Compact category lists in memory
         auto& map = this->ingredientsByEffect;
         for (auto it = map.begin(); it != map.end(); ++it) {
            auto& category = it->second;
            category.shrink_to_fit();
         }
      }
   }
//_MESSAGE("%s: Done", __FUNCTION__);
};
void XXNAlchemyMenu::UpdateSkill() {
//_MESSAGE("%s: Start", __FUNCTION__);
   auto skill = CALL_MEMBER_FN((RE::PlayerCharacter*)(*g_thePlayer), GetSkillLevel)(kActorVal_Alchemy);
   this->knownEffectCount = RE::AlchemySkillLevelToRevealedEffectCount(skill);
//_MESSAGE("%s: Done", __FUNCTION__);
};
void XXNAlchemyMenu::UpdatePotion() {
//_MESSAGE("%s: Start", __FUNCTION__);
   this->UpdateSkill(); // update known effect count
   {  // Update "Deselect All" button state. We'll update "Cook Potion" later, after validating that all selected ingredients are actually compatible.
      bool any = false;
      for (UInt8 i = 0; i < std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value; i++) {
         if (this->selectedIngredients[i]) {
            any = true;
            break;
         };
      }
      CALL_MEMBER_FN(this->tileButtonDeselectAll, UpdateFloat)(kTileValue_user1, any ? 2.0F : 1.0F);
   }
   //
   constexpr auto ce_selectionCount = std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value;
   //
   auto player = (RE::PlayerCharacter*) *g_thePlayer;
   auto potion = this->potion;
   if (!potion)
      potion = this->potion = RE::AlchemyItem::Create();
   auto  potionItem       = (RE::MagicItem*)      &(potion->magicItem);
   auto  potionEffects    = (RE::EffectItemList*) &(potionItem->list);
   UInt8 knownEffectCount = this->knownEffectCount;
   {  // Reset menu's effects state.
      CALL_MEMBER_FN(potionEffects, Clear)();
      this->potionEffectList.container->DeleteChildren(); // UI
      this->activeEffects.clear();
   }
   //
   UInt32 ingredientCount = 0;
   MagicItem* magicItems[ce_selectionCount] = { nullptr, nullptr, nullptr, nullptr };
   float  weight = 0.0F; // potion weight should be the average of all ingredients' weights
//_MESSAGE("%s: Begin Loop 1: Get ingredient effect lists, and update list of effects compatible with the potion", __FUNCTION__);
   for (UInt8 i = 0; i < ce_selectionCount; i++) {
//_MESSAGE("%s: Loop 1 iteration %d", __FUNCTION__, i);
      MagicItem* ingredient;
      {
         auto data = this->selectedIngredients[i];
         if (!data)
            continue;
         auto type = data->type;
         if (type) {
         }
         auto item = OBLIVION_CAST(type, TESBoundObject, IngredientItem);
         if (!item)
            continue;
         weight += RE::GetFormWeight(item);
         ++ingredientCount;
         ingredient = &item->magicItem;
         magicItems[i] = ingredient;
      }
//_MESSAGE("%s: Got magic item; pointer is %08X.", __FUNCTION__, ingredient);
      auto ingrList = (RE::EffectItemList*) &ingredient->list;
      //
      // Update active effects
      //
      UInt8 j = 0;
      auto effect = &ingrList->effectList;
      do {
//_MESSAGE("%s: Looping over ingredient %d: effect #%d (effect is at %08X)", __FUNCTION__, i, j, effect);
         if (j++ >= knownEffectCount) // j incremented after compare
            continue;
         auto effectItem = effect->effectItem;
         if (!effectItem)
            continue;
         {
            auto& list = this->activeEffects;
            if (std::find(list.begin(), list.end(), effectItem) == list.end()) { // why isn't there a "contains" shortcut for this?
               list.push_front(effectItem);
//_MESSAGE("%s: Added effect to this->activeEffects.", __FUNCTION__);
            }
         }
      } while (effect = effect->next);
   }
//_MESSAGE("%s: End Loop 1", __FUNCTION__);
   {
      if (ingredientCount)
         weight /= ingredientCount;
      else
         weight = 0.0F;
   }
   //
   UInt32 addedCount = 0;
   if (CALL_MEMBER_FN(player, GetSkillMasteryLevelBySkill)(kActorVal_Alchemy) >= RE::kSkillMastery_Master && ingredientCount == 1) {
//_MESSAGE("%s: Player is a master alchemist and has only one ingredient selected; skip Loop 2 in favor of single-ingredient potion behavior.", __FUNCTION__);
      MagicItem* which = nullptr;
      UInt8 i = 0;
      do {
         if (magicItems[i]) {
            which = magicItems[i];
            break;
         }
      } while (++i < std::extent<decltype(magicItems)>::value);
      RE::EffectItem* esi = CALL_MEMBER_FN((RE::EffectItemList*) &(which->list), GetEffectItemByIndex)(0);
      if (esi) {
         CALL_MEMBER_FN(potionEffects, AddItem)(RE::EffectItem::Clone(esi));
         addedCount = 1;
      }
//_MESSAGE("%s: Single-ingredient potion behavior done.", __FUNCTION__);
   } else {
      //
      // Outer two for-loops are unrolled in Bethesda's compiled code.
      //
      // Given ingredients ABCD we want to compare their effects like so:
      //  - A with B
      //  - A with C
      //  - A with D
      //  - B with C
      //  - B with D
      //  - C with D
      //
      // As such, the outer loop should run an iterator (i) over [0, 2] (since we have nothing 
      // to compare a left-side D to), and the inner loop should run over [i + 1, 3].
      //
      // ...Although it seems like Bethesda ended up comparing more pairs than that? Somehow??
      //
//_MESSAGE("%s: Begin Loop 2: Add paired effects from selected ingredients to the potion.", __FUNCTION__);
      constexpr auto ce_itemCount     = std::extent<decltype(magicItems)>::value;
      constexpr auto ce_itemCountLess = std::extent<decltype(magicItems)>::value - 1;
      for (UInt8 aIndex = 0; aIndex < ce_itemCountLess; aIndex++) {
//_MESSAGE("%s: Loop 2 Outer, iteration %d", __FUNCTION__, aIndex);
         if (auto aMagic = magicItems[aIndex]) {
            auto aList = (RE::EffectItemList*) &(aMagic->list);
            for (UInt8 bIndex = aIndex + 1; bIndex < ce_itemCount; bIndex++) {
//_MESSAGE("%s: Loop 2 Inner, iteration %d:%d", __FUNCTION__, aIndex, bIndex);
               if (auto bMagic = magicItems[bIndex]) {
                  auto bList  = (RE::EffectItemList*) &(bMagic->list);
                  UInt8 i = 0;
//_MESSAGE("%s: About to compare effects between ingredients %d and %d...", __FUNCTION__, aIndex, bIndex);
                  do {
                     auto iEffect = CALL_MEMBER_FN(aList, GetEffectItemByIndex)(i);
                     if (!iEffect)
                        continue;
                     UInt8 j = 0;
                     do {
//_MESSAGE("%s: Comparing effect %d[%d] with effect %d[%d]...", __FUNCTION__, aIndex, i, bIndex, j);
                        auto jEffect = CALL_MEMBER_FN(bList, GetEffectItemByIndex)(j);
                        if (!jEffect)
                           continue;
//_MESSAGE("%s: Effect %d[%d] exists.", __FUNCTION__, bIndex, j);
                        if (!CALL_MEMBER_FN(iEffect, SameAlchemyEffectAs)(jEffect))
                           continue;
//_MESSAGE("%s: Effects have the same alchemy effect.", __FUNCTION__);
                        if (CALL_MEMBER_FN(potionEffects, HasEffectMatchingAlch)(iEffect->effectCode, iEffect->actorValueOrOther))
                           continue;
//_MESSAGE("%s: Alchemy effect does not yet exist on the potion. It will be added.", __FUNCTION__);
                        auto copied = RE::EffectItem::Clone(iEffect);
                        CALL_MEMBER_FN(potionEffects, AddItem)(copied);
                        addedCount++;
                     } while (++j < knownEffectCount);
                  } while (++i < knownEffectCount);
               }
            }
         }
      }
//_MESSAGE("%s: End Loop 2. %d effects added.", __FUNCTION__, addedCount);
   }
   //
   // Following code is modeled on 0x0059485E.
   //
   static_assert(std::extent<decltype(XXNAlchemyMenu::apparati)>::value >= 3, "XXNAlchemyMenu::UpdatePotion is only programmed to work with at least four alchemy apparati available.");
   SInt8 qualities[std::extent<decltype(XXNAlchemyMenu::apparati)>::value];
   bool  hasPosEffects = false;
   float revealedEffectCount = 0.0F;
   {
      ExtraContainerChanges::EntryData* ebx = this->apparati[0];
      qualities[0] = /*SInt8*/ RE::GetFormQuality(ebx->type);
   }
   if (CALL_MEMBER_FN(potionEffects, HasNonPoisonousEffects)())
      hasPosEffects = true;
//_MESSAGE("%s: Ran HasNonPoisonousEffects check; result is %d.", __FUNCTION__, hasPosEffects);
   if (auto eax = this->apparati[1])
      qualities[1] = RE::GetFormQuality(eax->type);
   if (auto eax = this->apparati[3])
      qualities[3] = RE::GetFormQuality(eax->type);
   if (auto eax = this->apparati[2])
      qualities[2] = RE::GetFormQuality(eax->type);
   UInt8 esi = 0;
   float luckModifiedSkill   = CALL_MEMBER_FN(player, GetLuckModifiedSkill)(kActorVal_Alchemy);
   float mortarAndPestleMult = RE::ApplyMortarAndPestleMult(qualities[0], luckModifiedSkill);
//_MESSAGE("%s: Got mortalAndPestleMult; it's %f.", __FUNCTION__, mortarAndPestleMult);
   this->potion->goldValue = (SInt32) (mortarAndPestleMult * RE::GMST::fPotionGoldValueMult->f);
   this->potion->moreFlags |= RE::AlchemyItem::kAlchemy_NoAutocalc;
   this->potion->weight.weight = weight;
   {
      auto& tile = this->tileItemStats;
      //
      // We want to display the weight and gold value. However, we can't write raw numbers into the 
      // tile state. This is because if you attempt to display a floating-point value as a string, 
      // it gets truncated. We need to stringify these values manually.
      //
      // We stringify these values consistently with ExtraContainerChanges::EntryData::Subroutine0048F450, 
      // which is used to display values and weights in the InventoryMenu.
      //
      char  buf[12];
      float value = weight;
      {
         value = min(999.99F, value);
         if (value >= 1.0F) {
            SInt32 i = (SInt32) value;
            snprintf(buf, sizeof(buf), "%d", i);
         } else {
            snprintf(buf, sizeof(buf), "%.1f", RE::RoundToMultipleOf(value, 0.1F));
         }
      }
      CALL_MEMBER_FN(tile, UpdateString)(kItemStatsTrait_PotionWeight, buf);
      //
      value = this->potion->goldValue;
      {
         value = min(999.99F, value);
         if (value >= 1.0F) {
            SInt32 i = (SInt32) value;
            snprintf(buf, sizeof(buf), "%d", i);
         } else {
            snprintf(buf, sizeof(buf), "%.1f", RE::RoundToMultipleOf(value, 0.1F));
         }
      }
      CALL_MEMBER_FN(tile, UpdateString)(kItemStatsTrait_PotionValue, buf);
   }
   UInt32 i = 0;
//_MESSAGE("%s: Begin Loop 3: Compute final strength of each effect, and render it to the list (latter commented out).", __FUNCTION__);
   do { // at 0x0059494F
//_MESSAGE("%s: Loop 3 effect %d...", __FUNCTION__, i);
      float duration = 1.0F; // duration
      float magnitude = 1.0F; // magnitude
      auto edi = CALL_MEMBER_FN(potionEffects, GetEffectItemByIndex)(i);
      if (edi) { // at 0x0059496C
         EffectSetting* ecx = edi->setting;
         bool isHostile = CALL_MEMBER_FN(edi, IsHostile)();
         if (isHostile && this->apparati[1]) {
            esi = this->apparati[2] ? 3 : 0;
            esi += 2;
         } else {
            if (this->apparati[1] && this->apparati[3]) {
               esi = this->apparati[2] ? 3 : 0;
               esi += 1;
            } else {
               if (this->apparati[2])
                  esi = 3;
            }
         }
//_MESSAGE("%s: Unknown enum for potion strength is %d.", __FUNCTION__, esi);
         float baseCost = ecx->baseCost; // at 0x005949CD
         bool  noDuration;
         bool  noMagnitude;
         {
            auto effectFlags = ecx->effectFlags;
            noDuration  = effectFlags & EffectSetting::kEffect_NoDuration;
            noMagnitude = effectFlags & EffectSetting::kEffect_NoMagnitude;
         }
         if (!noDuration && !noMagnitude) { // at 0x005949E0
            RE::ComputeType1PotionEffectStrength(&magnitude, &duration, baseCost, mortarAndPestleMult, esi, isHostile, qualities[3], qualities[1], qualities[2], hasPosEffects);
         } else { // at 0x00594A2B
            if (noMagnitude) // at 0x00594A3C
               RE::ComputeType2PotionEffectStrength(&duration, baseCost, mortarAndPestleMult, esi, isHostile, qualities[3], qualities[1], qualities[2], hasPosEffects);
            else
               RE::ComputeType3PotionEffectStrength(&magnitude, baseCost, mortarAndPestleMult, esi, isHostile, qualities[3], qualities[1], qualities[2], hasPosEffects);
         }
         SInt32 finalDuration  = (SInt32) round(duration);
         SInt32 finalMagnitude = (SInt32) round(magnitude);
//_MESSAGE("%s: Final duration and magnitude are %d and %d.", __FUNCTION__, finalDuration, finalMagnitude);
         CALL_MEMBER_FN(edi, SetDurationIfPossible )(max((SInt32) finalDuration,  1)); // at 0x00594B47
         CALL_MEMBER_FN(edi, SetMagnitudeIfPossible)(max((SInt32) finalMagnitude, 1)); // at 0x00594B62
         CALL_MEMBER_FN(edi, SetAreaIfPossible)(0);
         { // Code in this block is modeled off of AlchemyMenu::RenderPotionEffect
            auto container = this->potionEffectList.container;
            auto listItem  = CALL_MEMBER_FN(this, CreateTemplatedTile)(container, "potion_effect_template", nullptr);
            if (listItem) {
               RE::BSStringT temporary;
               CALL_MEMBER_FN(listItem, UpdateFloat) (kTileValue_listindex, i);
               CALL_MEMBER_FN(edi,      FormatDescription)(&temporary, potionItem, 1.0F);
               CALL_MEMBER_FN(listItem, UpdateString)(kPotionEffectTrait_Description, temporary.m_data);
               CALL_MEMBER_FN(edi,      GetScriptedOrActualName)(&temporary);
               CALL_MEMBER_FN(&(listItem->name), Replace_MinBufLen)(temporary.m_data, 0);
               const char* icon = edi->setting->texture.ddsPath.m_data;
               if (!icon)
                  icon = "";
               char iconPath[255];
               snprintf(iconPath, sizeof(iconPath), "%s\\%s", "Icons", icon);
               CALL_MEMBER_FN(listItem, UpdateString)(kPotionEffectTrait_IconPath, iconPath);
//_MESSAGE("%s: Rendered effect.", __FUNCTION__);
            }
         }
         //
         // TODO: This handling for the text field is WRONG WRONG WRONG WRONG WRONG but I don't want to bother with it yet
         //
         if (this->potionNameState != kTextboxState_Custom && this->potionNameState != kTextboxState_NewPotion) { // at 0x00594B97
            if (!i) { // name the potion based on its first effect, right? ebx == 0?
               char displayName[0x60]; // TODO: Can we verify the proper size for this?
               CALL_MEMBER_FN(edi, GetName)(displayName); // TODO: Actually, I'm not super sure this writes to the arg; arg may be const char** outPtr instead; check other callers?
               CALL_MEMBER_FN(this->textInputState, SetStringsTo)(displayName);
               this->potionNameState = kTextboxState_Auto;
               this->UpdateTextField();
            }
         }
         CALL_MEMBER_FN(this->tileButtonCreatePotion, UpdateFloat)(kTileValue_user1, 2.0F);
      } else { // at 0x00594BFE
//_MESSAGE("%s: Effect %d is missing!!", __FUNCTION__, i);
         if (i == 0) { // If the first magic effect in the potion's list is invalid, assume they all are and abort?
            CALL_MEMBER_FN(this->textInputState, SetStringsTo)(RE::GMST::sNamePotion->s);
            this->potionNameState = kTextboxState_Empty;
            this->UpdateTextField();
            //
            CALL_MEMBER_FN(this->tileButtonCreatePotion, UpdateFloat)(kTileValue_user1, 1.0F); // disable the button
//_MESSAGE("%s: Aborting potion update!", __FUNCTION__);
            return;
         }
      }
   } while (++i < addedCount); // at 0x00594C2F
//_MESSAGE("%s: Loop 3 done", __FUNCTION__);
   //
   auto customName = CALL_MEMBER_FN(this->tileNameText, GetStringTraitValue)(kTileValue_string);
   CALL_MEMBER_FN((RE::BSStringT*) &(this->potion->magicItem.name), Replace_MinBufLen)(customName, 0);
   this->UpdateRenderedInventory();
//_MESSAGE("%s: Done", __FUNCTION__);
};
void XXNAlchemyMenu::CreatePotion() {
//_MESSAGE("%s: Start", __FUNCTION__);
   (*g_thePlayer)->ModExperience(0x13, 0, 0.0F);
   (*g_thePlayer)->miscStats[0x30]++;
   UInt32 dataHandlerResult;
   {  // Fire OBSE hook and run data-handler code
      auto dummy = (UInt32*) malloc(0x98);
      *(RE::AlchemyItem**)(dummy + 0x94) = this->potion;
      dataHandlerResult = ThisStdCall(0x00594CA0, dummy); // WARNING: If we haven't patched AlchemyMenu::CookPotion to NOP out everything unrelated to OBSE's handler, then this will almost certainly crash!
      delete dummy;
   }
//_MESSAGE("%s: OBSE hook and data handler call therein returned result %08X", __FUNCTION__, dataHandlerResult);
   if (dataHandlerResult) {
      //
      // A base form matching our potion already exists, and has been returned. We'll reuse it.
      //
      ThisStdCall(0x004D8720, *g_thePlayer, dataHandlerResult, 0, 1); // add item?
//_MESSAGE("%s: Potion successfully added to player's inventory (if that's what that call does?).", __FUNCTION__);
   } else {
      //
      // No potion like ours exists yet. We need to create and use a new base form.
      //
//_MESSAGE("%s: About to commit new base potion to DH", __FUNCTION__);
      ThisStdCall(0x0044D950, *g_dataHandler, this->potion);
//_MESSAGE("%s: About to commit new base potion to CBOL", __FUNCTION__);
      ThisStdCall(0x00459800, *g_createdBaseObjList, this->potion);
      //this->potion->weight.weight = 0.0F; // vanilla does it in AlchemyMenu::CookPotion; we do it in our UpdatePotion instead
      if (CALL_MEMBER_FN((RE::EffectItemList*) &(this->potion->magicItem.list), HasNonPoisonousEffects)()) { // TODO: Obviously we got the name on that method backwards; this also has implications for other stuff we decoded, in UpdatePotion, as well
         this->potion->model.SetModelPath("Clutter\\Potions\\PotionPoison.NIF");
         this->potion->icon.SetPath("Clutter\\Potions\\IconPotionPoison01.dds");
      } else {
         this->potion->model.SetModelPath("Clutter\\Potions\\Potion01.NIF");
         this->potion->icon.SetPath("Clutter\\Potions\\IconPotion01.dds");
      }
      ThisStdCall(0x004D8720, *g_thePlayer, this->potion, 0, 1); // add item?
//_MESSAGE("%s: Potion successfully added to player's inventory (if that's what that call does?).", __FUNCTION__);
      this->potion = RE::AlchemyItem::Create();
//_MESSAGE("%s: Gave ourselves a fresh current working potion", __FUNCTION__);
   }
   QueueUIMessage(RE::GMST::sPotionSuccess->s, 0, 1, -1.0F);
   RE::PlayUIClicksound(0x12);
   this->potionNameState = kTextboxState_NewPotion;
   {
      bool mustRedrawEntirely = false;
      UInt8 i = 0;
      do {
         auto current = this->selectedIngredients[i];
         if (!current)
            continue;
         (*g_thePlayer)->RemoveItem(current->type, nullptr, 1, false, false, nullptr, nullptr, nullptr, 1, 1);
         if (current->countDelta == 1) {
//_MESSAGE("%s: Item stack gone; we'll need to update the inventory from scratch", __FUNCTION__);
            mustRedrawEntirely = true;
         } else {
            current->countDelta--;
         }
      } while (++i < std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value);
      if (mustRedrawEntirely) {
//_MESSAGE("%s: Loop done; updating inventory from scratch", __FUNCTION__);
         this->UpdateInventory();
         this->RenderInventory();
         this->RenderEffectList();
      } else {
//_MESSAGE("%s: Loop done; stacks unaltered, so we can just fiddle with rendered counts; will do", __FUNCTION__);
         this->UpdateRenderedInventory();
      }
   }
   this->UpdatePotion();
};

bool XXNAlchemyMenu::IngredientHasEffect(IngredientItem* ingredient, UInt32 effectCode) {
   auto effect = &(ingredient->magicItem.list.effectList);
   for (UInt8 i = 0; i < this->knownEffectCount; i++) {
      auto effectItem = effect->effectItem;
      if (effectItem && effectItem->effectCode == effectCode)
         return true;
      effect = effect->next;
      if (!effect)
         break;
   }
   return false;
};
bool XXNAlchemyMenu::IngredientIsCompatible(IngredientItem* ingredient) {
   auto count = this->knownEffectCount;
   auto& list = this->activeEffects;
   if (list.empty()) {
      //
      // No ingredients are selected. Ingredients are "compatible" (i.e. eligible for use) if:
      //
      //  - The player is a master alchemist, capable of creating a single-ingredient potion
      //
      //    OR
      //
      //  - The player has at least one other ingredient with an effect in common
      //
      if (CALL_MEMBER_FN((RE::PlayerCharacter*) *g_thePlayer, GetSkillMasteryLevelBySkill)(kActorVal_Alchemy) >= RE::kSkillMastery_Master)
         return true;
      auto current = &(ingredient->magicItem.list.effectList);
      for (UInt8 i = 0; i < count; i++) {
         auto effectItem = (RE::EffectItem*) current->effectItem;
         EffectCodeAndAV code = XXNAlchemyMenu::GetCombinedIdentifier(effectItem);
         if (!code)
            continue;
         try {
            auto& category = this->ingredientsByEffect.at(code);
            if (category.size() > 1)
               return true;
         } catch (std::out_of_range&) {}
      }
      return false;
   }
   for (auto it = list.begin(); it != list.end(); ++it) {
      auto effect  = *it;
      auto activeCode = effect->effectCode;
      auto activeAV   = effect->actorValueOrOther;
      //
      auto  current = &(ingredient->magicItem.list.effectList);
      UInt8 i = 0;
      do {
         if (++i > count)
            break;
         auto effectItem = current->effectItem;
         auto code = effectItem->effectCode;
         auto actorVal = effectItem->actorValueOrOther;
         if (effectItem->effectCode == activeCode && effectItem->actorValueOrOther == activeAV)
            return true;
      } while (current = current->next);
   }
   return false;
};

XXNAlchemyMenu::EffectCodeAndAV XXNAlchemyMenu::GetEffectAndAVFromListItem(RE::Tile* listItem) const {
   EffectCodeAndAV effectAndAV = 0;
   UInt32 effectCode;
   UInt32 avOrOther;
   {
      auto code = CALL_MEMBER_FN(listItem, GetStringTraitValue)(kInventoryCategoryTrait_EffectCode); // the effect code written as a const char[5]
      if (!code)
         return 0;
      effectCode = *((UInt32*) code); // NOTE: Don't bswap; it's stringified with the right endianness as is
      if (!effectCode || (code[0] == ' ' && code[1] == '\0')) // RE::Tile::UpdateString(traitID, "") actually stores " " instead.
         return 0;
   }
   avOrOther = (UInt32) CALL_MEMBER_FN(listItem, GetFloatTraitValue)(kInventoryCategoryTrait_AVOrOther);
   effectAndAV = effectCode;
   effectAndAV |= ((EffectCodeAndAV) avOrOther) << 32;
   return effectAndAV;
};
RE::ExtraContainerChanges::EntryData* XXNAlchemyMenu::GetIngredientFromListItem(RE::Tile* listItem) const {
   auto& list = this->ingredients;
   UInt32 count = list.size();
   SInt32 index;
   {
      if (!CALL_MEMBER_FN(listItem, GetTrait)(kTileValue_listindex)) // limit to auto-generated
         return nullptr;
      float temp;
      if (!listItem->GetFloatValue(kInventoryTrait_ItemRawIndex, &temp))
         return nullptr;
      index = temp;
      if (index >= count)
         return nullptr;
   }
   return list.at(index).data;
};

void XXNAlchemyMenu::RenderEffectList() {
//_MESSAGE("%s: Start", __FUNCTION__);
   //
   // Masters can craft single-ingredient potions that have only the ingredient's first effect. 
   // As such, if no ingredients are selected, then any ingredient is selectable because any 
   // ingredient can, after being selected, be used for a single-ingredient potion. Check for 
   // this outside of our main loop.
   //
   UInt8  masteryLevel = 0;
   UInt32 ingredientCount = 0;
   for (UInt8 i = 0; i < 4; i++)
      if (this->selectedIngredients[i])
         ingredientCount++;
   if (!ingredientCount)
      masteryLevel = CALL_MEMBER_FN((RE::PlayerCharacter*) *g_thePlayer, GetSkillMasteryLevelBySkill)(kActorVal_Alchemy);
   //
   auto container = this->effectList.container;
   container->DeleteChildren();
   UInt32 listindex = 0;
   RE::Tile* allCats;
   bool   anyUsable = false;
   {  // Add the "All Ingredients" list item.
      allCats = CALL_MEMBER_FN(this, CreateTemplatedTile)(container, "effect_list_item_template", nullptr);
      if (allCats) {
         CALL_MEMBER_FN(allCats, UpdateFloat) (kTileValue_id, kTileID_InventoryShowAllCategories);
         CALL_MEMBER_FN(allCats, UpdateFloat) (kTileValue_listindex, (float) (listindex++));
         CALL_MEMBER_FN(allCats, UpdateString)(kInventoryCategoryTrait_Name, "All Ingredients");
         CALL_MEMBER_FN(allCats, UpdateFloat) (kInventoryCategoryTrait_IsSelected, this->selectedEffect ? 1.0F : 2.0F);
         CALL_MEMBER_FN(allCats, UpdateString)(kInventoryCategoryTrait_EffectCode, " ");
         CALL_MEMBER_FN(allCats, UpdateFloat) (kInventoryCategoryTrait_AVOrOther, 0.0F);
         CALL_MEMBER_FN(allCats, UpdateFloat) (kInventoryCategoryTrait_IsUsable, 1.0F); // will be overridden later if any categories are usable
      }
   }
   UInt32 effect = this->selectedEffect;
   auto&  map = this->availableEffects;
   for (auto it = map.begin(); it != map.end(); ++it) {
      auto code = it->code;
      auto name = it->name.c_str();
      RE::Tile* listItem = CALL_MEMBER_FN(this, CreateTemplatedTile)(container, "effect_list_item_template", nullptr);
      if (!listItem)
         continue;
      CALL_MEMBER_FN(listItem, UpdateFloat)(kTileValue_id, kTileID_InventoryCategory);
      CALL_MEMBER_FN(listItem, UpdateFloat)(kTileValue_listindex, (float) (listindex++));
      CALL_MEMBER_FN(listItem, UpdateString)(kInventoryCategoryTrait_Name, name);
      CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryCategoryTrait_IsSelected, code == effect ? 2.0F : 1.0F);
      {
         bool compat = false;
         if (masteryLevel >= RE::kSkillMastery_Master) { // ingredientCount must also == 0; we only set masteryLevel if ingredientCount == 0
            compat = true; // master crafting is available
         } else {
            try {
               if (this->ingredientsByEffect.at(code).size() > 1) {
                  auto& activeEffects = this->activeEffects;
                  if (activeEffects.empty())
                     compat = true; // if no ingredients are selected, then there are no constraints on usable effects other than that the player must have more than two ingredients with the effect (outside of master crafting)
                  else
                     for (auto it = activeEffects.begin(); it != activeEffects.end(); ++it)
                        if (XXNAlchemyMenu::GetCombinedIdentifier(*it) == code) {
                           compat = true;
                           break;
                        }
               }
            } catch (std::out_of_range&) {};
         }
         CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryCategoryTrait_IsUsable, compat ? 2.0F : 1.0F); // has any usable items
         if (compat)
            anyUsable = true;
      }
      {  // Used so that we can tell what effect was clicked in HandleMouseUp
         char codeStr[5];
         *((UInt32*) codeStr) = code;
         codeStr[4] = '\0';
         CALL_MEMBER_FN(listItem, UpdateString)(kInventoryCategoryTrait_EffectCode, codeStr);
      }
      {
         UInt32 av = code >> 32;
         CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryCategoryTrait_AVOrOther, av);
      }
   }
   if (anyUsable)
      CALL_MEMBER_FN(allCats, UpdateFloat)(kInventoryCategoryTrait_IsUsable, 2.0F);
//_MESSAGE("%s: Done", __FUNCTION__);
};
void XXNAlchemyMenu::RenderInventory() {
//_MESSAGE("%s: Start", __FUNCTION__);
   auto container = this->ingredientList.container;
   container->DeleteChildren();
   auto potionEffects = (RE::EffectItemList*) &(this->potion->magicItem.list);
//_MESSAGE("%s: Getting list... Selected effect is %d.", __FUNCTION__, this->selectedEffect);
   ItemList* list = &this->ingredients;
   {
      auto code = this->selectedEffect;
      if (code)
         try {
            list = &this->ingredientsByEffect.at(code);
         } catch (std::out_of_range&) {}
   }
//_MESSAGE("%s: Got list.", __FUNCTION__);
   UInt32 size   = list->size();
   UInt32 effect = this->selectedEffect;
   UInt32 listindex = 0;
   for (UInt32 i = 0; i < size; i++) {
//_MESSAGE("%s: Iteration %d", __FUNCTION__, i);
      auto& item = list->at(i);
      auto  data = item.data;
      RE::Tile* listItem = CALL_MEMBER_FN(this, CreateTemplatedTile)(container, "ingredient_list_item_template", nullptr);
      if (!listItem)
         continue;
      CALL_MEMBER_FN(listItem, UpdateFloat)(kTileValue_id, kTileID_InventoryItem);
      CALL_MEMBER_FN(listItem, UpdateFloat)(kTileValue_listindex, (float) (listindex++));
      {  // Give the new tile the item name, and set the tile's own name to the item name (capped to 256 chars) with spaces replaced with underscores.
         const char* itemName = CALL_MEMBER_FN(data, GetName)(); // PLACEHOLDER
         CALL_MEMBER_FN(listItem, UpdateString)(kInventoryTrait_ItemName, itemName);
         char tileName[256];
         {
            for(UInt8 f = 0; f < 256; f++) {
               char c = itemName[f];
               if (c == ' ')
                  c = '_';
               tileName[f] = c;
               if (c == '\0')
                  break;
            }
            tileName[255] = '\0';
         }
         CALL_MEMBER_FN((RE::BSStringT*) &listItem->name, Replace_MinBufLen)(itemName, 0);
      }
      {  // ingredient icon
         const char* icon = CALL_MEMBER_FN(data, GetIcon)((RE::Actor*) *g_thePlayer); // TODO: double-check that this doesn't need to be deleted manually by the caller
         if (!icon)
            icon = "";
         char iconPath[255];
         snprintf(iconPath, sizeof(iconPath), "%s\\%s", "Icons", icon);
         CALL_MEMBER_FN(listItem, UpdateString)(kInventoryTrait_ItemIcon, iconPath);
      }
      CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryTrait_ItemCount, data->countDelta);
      {
         bool found = false;
         auto& list = this->selectedIngredients;
         for (UInt8 i = 0; i < std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value; i++) {
            if (list[i] == data) {
               found = true;
               break;
            }
         }
         CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryTrait_ItemIsSelected, found ? 2.0F : 1.0F);
      }
      CALL_MEMBER_FN(listItem, UpdateFloat) (kInventoryTrait_ItemRawIndex, item.index);
      CALL_MEMBER_FN(listItem, UpdateFloat) (kTileValue_user12, 0.0F); // PLACEHOLDER // TODO: handle stolen
      CALL_MEMBER_FN(listItem, UpdateString)(kTileValue_user13, ""); // PLACEHOLDER// TODO: handle stolen
      CALL_MEMBER_FN(listItem, UpdateFloat) (kInventoryTrait_ItemArrayIndex, i);
      {
         bool compat = this->IngredientIsCompatible((IngredientItem*) data->type);
         CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryTrait_ItemIsUsable, compat ? 2.0F : 1.0F); // compatible with potion?
      }
   }
//_MESSAGE("%s: Done", __FUNCTION__);
};
void XXNAlchemyMenu::UpdateRenderedEffectList() {
   //
   // Masters can craft single-ingredient potions that have only the ingredient's first effect. 
   // As such, if no ingredients are selected, then any ingredient is selectable because any 
   // ingredient can, after being selected, be used for a single-ingredient potion. Check for 
   // this outside of our main loop.
   //
   UInt8  masteryLevel = 0;
   UInt32 ingredientCount = 0;
   for (UInt8 i = 0; i < 4; i++)
      if (this->selectedIngredients[i])
         ingredientCount++;
   if (!ingredientCount)
      masteryLevel = CALL_MEMBER_FN((RE::PlayerCharacter*) *g_thePlayer, GetSkillMasteryLevelBySkill)(kActorVal_Alchemy);
   //
   auto      container = this->effectList.container;
   auto      child     = container->childList.start;
   RE::Tile* allCats   = nullptr;
   bool      anyValid  = false;
   for (; child; child = child->next) {
      auto tile = child->data;
      if (!tile)
         continue;
      auto code = this->GetEffectAndAVFromListItem(tile);
      if (!code) { // either a bad list item or the "All" list item
         if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_id) == kTileID_InventoryShowAllCategories) {
            allCats = tile;
            CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryCategoryTrait_IsUsable, 2.0F);
            CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryCategoryTrait_IsSelected, this->selectedEffect ? 1.0F : 2.0F);
         }
         continue;
      }
      bool compat = false;
      if (masteryLevel >= RE::kSkillMastery_Master) { // ingredientCount must also == 0; we only set masteryLevel if ingredientCount == 0
         compat = true; // master crafting is available
      } else {
         try {
            if (this->ingredientsByEffect.at(code).size() > 1) {
               auto& activeEffects = this->activeEffects;
               if (activeEffects.empty())
                  compat = true; // if no ingredients are selected, then there are no constraints on usable effects other than that the player must have more than two ingredients with the effect (outside of master crafting)
               else
                  for (auto it = activeEffects.begin(); it != activeEffects.end(); ++it)
                     if (XXNAlchemyMenu::GetCombinedIdentifier(*it) == code) {
                        compat = true;
                        break;
                     }
            }
         } catch (std::out_of_range&) {};
      }
      if (compat)
         anyValid = true;
      CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryCategoryTrait_IsUsable,   compat ? 2.0F : 1.0F); // has any usable items
      CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryCategoryTrait_IsSelected, code == this->selectedEffect ? 2.0F : 1.0F);
   }
   if (allCats)
      CALL_MEMBER_FN(allCats, UpdateFloat)(kInventoryCategoryTrait_IsUsable, anyValid ? 2.0F : 1.0F);
};
void XXNAlchemyMenu::UpdateRenderedInventory() {
   ItemList* list = &this->ingredients; // don't apply the filter; ItemAndIndex stuff is relative to the "all items" list
   auto count     = list->size();
   auto container = this->ingredientList.container;
   auto child     = container->childList.start;
   for (; child; child = child->next) {
      auto tile = child->data;
      if (!tile)
         continue;
      auto data = this->GetIngredientFromListItem(tile);
      if (!data)
         continue;
      bool compat = this->IngredientIsCompatible((IngredientItem*) data->type);
      CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryTrait_ItemCount, data->countDelta);
      CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryTrait_ItemIsUsable, compat ? 2.0F : 1.0F); // compatible with potion?
      {
         bool found = false;
         auto& list = this->selectedIngredients;
         for (UInt8 i = 0; i < std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value; i++) {
            if (list[i] == data) {
               found = true;
               break;
            }
         }
         CALL_MEMBER_FN(tile, UpdateFloat)(kInventoryTrait_ItemIsSelected, found ? 2.0F : 1.0F);
      }
   }
};
void XXNAlchemyMenu::UpdateTextField() {
   auto& state = this->textInputState;
   auto  name  = (RE::BSStringT*) &(this->potion->magicItem.name);
   CALL_MEMBER_FN(this->tileNameText, UpdateString)(kTileValue_string, CALL_MEMBER_FN(state, UpdateRenderedStringForCursorMovement)());
   if (state->hasFocus || this->potionNameState == kTextboxState_Empty)
      return;
   if (auto custom = CALL_MEMBER_FN(state, GetEditValue)()) {
      CALL_MEMBER_FN(name, Replace_MinBufLen)(custom, 0);
      return;
   }
   if (name->GetLength()) {
      auto eax = name->m_data;
      if (!eax)
         eax = "";
      CALL_MEMBER_FN(state, SetStringsTo)(eax);
      CALL_MEMBER_FN(this->tileNameText, UpdateString)(kTileValue_string, eax);
      return;
   }
   const char* str = RE::GMST::sNamePotion->s;
   CALL_MEMBER_FN(this->textInputState, SetStringsTo)(str);
   CALL_MEMBER_FN(this->tileNameText,   UpdateString)(kTileValue_string, str);
   this->potionNameState = kTextboxState_Empty;
};

RE::Tile* ShowXXNAlchemyMenu(RE::ExtraContainerChanges::EntryData* appa1, RE::ExtraContainerChanges::EntryData* appa2, RE::ExtraContainerChanges::EntryData* appa3, RE::ExtraContainerChanges::EntryData* appa4) {
   if (auto existing = GetMenuByType(0x410)) {
      ((XXNAlchemyMenu*) existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)(XXNAlchemyMenu::menuPath);
   auto menu = (XXNAlchemyMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu)
      return nullptr;
   if (menu->GetID() != XXNAlchemyMenu::kID) {
      menu->Dispose(true);
      return nullptr;
   }
   {  // Vital menu setup tasks:
      //
      // These tasks SHOULD be performed for us by TileMenu::UpdateField, but they aren't, and it seems 
      // like every ShowWhateverMenu subroutine does them manually anyway. If they're not performed, 
      // then a wide variety of major bugs will occur, including:
      //
      //  - The menu cannot be added to InterfaceManager::activeMenuIDs, and consequently cannot receive 
      //    keyboard input events or affect other menus (with respect to modals)
      //
      //  - The menu will not close properly if the user pauses and loads a savegame while it's open
      //
      CALL_MEMBER_FN(menu, RegisterTile)(tile);
      {  // set menu depth
         float stackingtype = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_stackingtype);
         if (stackingtype == 6006.0F || stackingtype == 102.0F) {
            float depth = RE::GetNewMenuDepth();
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_depth, depth);
         }
      }
   }
   menu->apparati[0] = appa1;
   menu->apparati[1] = appa2;
   menu->apparati[2] = appa3;
   menu->apparati[3] = appa4;
   for (auto i = 0; i < std::extent<decltype(menu->apparati)>::value; i++) {
      auto current = menu->apparati[i];
      auto curTile = menu->tileApparati[i];
      if (!current) { // apparatus is missing
         CALL_MEMBER_FN(curTile, UpdateString)(kTileValue_filename, "");
         CALL_MEMBER_FN(curTile, UpdateString)(kTileValue_string,   "");
         continue;
      }
      const char* icon = CALL_MEMBER_FN(current, GetIcon)((RE::Actor*) *g_thePlayer);
      if (!icon)
         icon = "";
      char iconPath[255];
      snprintf(iconPath, sizeof(iconPath), "%s\\%s", "Icons", icon);
      CALL_MEMBER_FN(curTile, UpdateString)(kTileValue_filename, iconPath);
      CALL_MEMBER_FN(curTile, UpdateString)(kTileValue_string,   CALL_MEMBER_FN(current, GetName)());
   }
   CALL_MEMBER_FN(menu->textInputState, SetStringsTo)(RE::GMST::sNamePotion->s);
   menu->UpdateTextField();
   menu->UpdateInventory();
   menu->RenderInventory();
   menu->RenderEffectList();
   CALL_MEMBER_FN(menu, EnableMenu)(0);
   return tile;
};