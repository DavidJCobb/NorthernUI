#include "XXNSampleInventoryMenu.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "obse/GameMenus.h"

XXNSampleInventoryMenu::ItemAndIndex::ItemAndIndex(RE::ExtraContainerChanges::EntryData* data, UInt32 index) {
   this->data  = data;
   this->index = index;
   this->name  = CALL_MEMBER_FN(data, GetName)();
};
XXNSampleInventoryMenu::ItemAndIndex::~ItemAndIndex() {
   if (this->data) {
      CALL_MEMBER_FN(this->data, Destructor)();
      FormHeap_Free(this->data);
   }
};
bool XXNSampleInventoryMenu::ItemAndIndex::operator<(const XXNSampleInventoryMenu::ItemAndIndex& rhs) const {
   return (_stricmp(this->name, rhs.name) < 0);
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

XXNSampleInventoryMenu* XXNSampleInventoryMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNSampleInventoryMenu));
   if (memory) {
      auto result = new (memory) XXNSampleInventoryMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNSampleInventoryMenu::XXNSampleInventoryMenu() {
};
XXNSampleInventoryMenu::~XXNSampleInventoryMenu() {
   this->ClearInventory();
};

void XXNSampleInventoryMenu::Dispose(bool free) {
   this->~XXNSampleInventoryMenu();
   if (free)
      FormHeap_Free(this);
};
void XXNSampleInventoryMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
   if (newID == kTileID_Background) {
      this->tileBackground = tile;
   } else if (newID == kTileID_ButtonExit) {
      this->tileButtonExit = tile;
   } else if (newID == kTileID_ListContainer) {
      this->listContainer = tile;
   } else if (newID == kTileID_ListScrollBar) {
      this->listScrollBar = tile;
   } else if (newID == kTileID_ListScrollThumb) {
      this->listScrollThumb = tile;
   } else if (newID == kTileID_ItemCount) {
      this->tileItemCount = tile;
   }
};
void XXNSampleInventoryMenu::HandleMouseUp(SInt32 id, RE::Tile* target) {
   if (id == kTileID_ButtonExit) {
      if (CALL_MEMBER_FN(target, GetFloatTraitValue)(RE::kButtonTraitValue_Enabled) == 1.0F)
         return;
      this->Close();
      return;
   }
};
void XXNSampleInventoryMenu::HandleFrameMouseDown(SInt32 tileID, RE::Tile* target) {
   if (tileID == kTileID_ListScrollThumb) {
      Menu::HandleScrollbarThumbDrag(this->listScrollBar, this->listScrollThumb, RE::Menu::kScrollDragBehavior_Interval);
      return;
   }
};
void XXNSampleInventoryMenu::HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target) {
   if (target == this->listScrollBar || target == this->listContainer || target == this->listScrollThumb) {
      Menu::HandleScrollbarMouseWheel(this->listScrollThumb);
      return;
   }
};
UInt32 XXNSampleInventoryMenu::GetID() {
   return kID;
};

void XXNSampleInventoryMenu::ClearInventory() {
   this->inventory.clear();
};
void XXNSampleInventoryMenu::RenderInventory() {
   //
   // NOTE: This doesn't clear the list before rendering it; calling it multiple times would 
   // just append new copies of the list.
   //
   auto&  list = this->inventory;
   UInt32 size = list.size();
   UInt32 listindex = 0;
   for (UInt32 i = 0; i < size; i++) {
      auto& item = list[i];
      auto  data = item.data;
      if (!data || !data->countDelta)
         continue;
      RE::Tile* listItem = CALL_MEMBER_FN(this, CreateTemplatedTile)(this->listContainer, "inventory_list_item_template", nullptr);
      if (!listItem)
         continue;
      CALL_MEMBER_FN(listItem, UpdateFloat)(kTileValue_listindex, (float) (++listindex));
      {  // Give the new tile the item name, and set the tile's own name to the item name (capped to 256 chars) with spaces replaced with underscores.
         const char* itemName = CALL_MEMBER_FN(data, GetName)(); // PLACEHOLDER
         CALL_MEMBER_FN(listItem, UpdateString)(kInventoryTrait_ItemName, itemName);
         char tileName[256];
         {
            for (UInt8 f = 0; f < 256; f++) {
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
         const char* icon = CALL_MEMBER_FN(data, GetIcon)((RE::Actor*) *g_thePlayer);
         if (!icon)
            icon = "";
         char iconPath[255];
         snprintf(iconPath, sizeof(iconPath), "%s\\%s", "Icons", icon);
         CALL_MEMBER_FN(listItem, UpdateString)(kInventoryTrait_ItemIcon, icon);
      }
      CALL_MEMBER_FN(listItem, UpdateString)(kTileValue_user7, "Value"); // PLACEHOLDER
      CALL_MEMBER_FN(listItem, UpdateString)(kTileValue_user8, "Value"); // PLACEHOLDER
      CALL_MEMBER_FN(listItem, UpdateFloat) (kInventoryTrait_ItemPocket, 0); // PLACEHOLDER // 1, 2, 4, 8 (type enum)
      /*{
         bool found = false;
         auto& list = this->selectedIngredients;
         for (UInt8 i = 0; i < std::extent<decltype(XXNAlchemyMenu::selectedIngredients)>::value; i++) {
            if (list[i] == data) {
               found = true;
               break;
            }
         }
         CALL_MEMBER_FN(listItem, UpdateFloat)(kInventoryTrait_ItemIsSelected, found ? 2.0F : 1.0F);
      }*/
      CALL_MEMBER_FN(listItem, UpdateFloat) (kInventoryTrait_ItemRawIndex, item.index);
      CALL_MEMBER_FN(listItem, UpdateFloat) (kTileValue_user12, 0.0F); // PLACEHOLDER // TODO: handle stolen
      CALL_MEMBER_FN(listItem, UpdateString)(kTileValue_user13, ""); // PLACEHOLDER// TODO: handle stolen
      CALL_MEMBER_FN(listItem, UpdateFloat) (kInventoryTrait_ItemArrayIndex, i);
   }
   CALL_MEMBER_FN(this->tileItemCount, UpdateFloat)(kItemCountTrait_Count, listindex);
};
void XXNSampleInventoryMenu::UpdateInventory() {
   this->ClearInventory();
   //
   auto   player = (RE::PlayerCharacter*) *g_thePlayer; // cast so we can use stuff we've found
   UInt32 count = CALL_MEMBER_FN((RE::TESObjectREFR*) player, GetInventoryItemCount)(0);
   UInt32 i = 0;
   this->inventory.reserve(count);
   do {
      auto data = CALL_MEMBER_FN((RE::TESObjectREFR*) player, GetInventoryItemByIndex)(i, 0);
      if (data) {
         bool destroy = true;
         auto item = data->type;
         if (item && /*item->typeID == kFormType_Ingredient &&*/ !RE::FormIsGoldAsInCurrency(item) /*&& !item->TIsQuestItem()*/) {
            if (CALL_MEMBER_FN(data, Subroutine004854F0)(player, false, true, true, false)) { // filters inventory
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
      //
      // If an item of the same type already exists in our list, then merge this in.
      //
      bool  alreadyExisted = false;
      auto& list = this->inventory;
      for (auto it = list.begin(); it != list.end(); ++it) {
         auto existing = it->data;
         if (!existing)
            continue;
         if (existing->type == data->type) {
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
      //
      // ...Otherwise, add this new item to our list.
      //
      list.emplace(list.end(), data, i);
   } while (++i < count);
   //
   // Sort the list.
   //
   this->inventory.shrink_to_fit();
   std::sort(this->inventory.begin(), this->inventory.end()); // sort alphabetically
};

RE::Tile* ShowXXNSampleInventoryMenuInsteadOfAlchemyMenu(RE::ExtraContainerChanges::EntryData* appa1, RE::ExtraContainerChanges::EntryData* appa2, RE::ExtraContainerChanges::EntryData* appa3, RE::ExtraContainerChanges::EntryData* appa4) {
   if (auto existing = GetMenuByType(0x410)) {
      ((XXNSampleInventoryMenu*) existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)(XXNSampleInventoryMenu::menuPath);
   auto menu = (XXNSampleInventoryMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu) {
      return nullptr;
   }
   if (menu->GetID() != 0x410) {
      menu->Dispose(true);
      return nullptr;
   }
   {  // These tasks SHOULD be performed for us automatically, but they aren't, and it seems like every ShowWhateverMenu subroutine does them manually anyway
      CALL_MEMBER_FN(menu, RegisterTile)(tile);
      {  // set menu depth
         float stackingtype = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_stackingtype);
         if (stackingtype == 6006.0F || stackingtype == 102.0F) {
            float depth = RE::GetNewMenuDepth();
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_depth, depth);
         }
      }
   }
   menu->UpdateInventory();
   menu->RenderInventory();
   CALL_MEMBER_FN(menu, EnableMenu)(0);
   return tile;
};