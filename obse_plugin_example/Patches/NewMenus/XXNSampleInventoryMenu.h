#pragma once
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/ExtraData/ExtraContainerChanges.h"
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "ReverseEngineered/Forms/AlchemyItem.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameExtraData.h"
#include "obse/GameForms.h"

//
// This is a simple example menu set up to replace the native AlchemyMenu. It offers 
// the following functionality:
//
//  - Lists all of the player's playable items, using a template
//  - Sorts the items alphabetically by name (character code sort, case-insensitive)
//  - Uses a scrollable pane with one scrollbar
//  - Shows a text readout of the total number of (kinds of) items being displayed
//  - Offers a "Close" button
//

class XXNSampleInventoryMenu : public RE::Menu {
   public:
      enum { kID = 0x410 };
      XXNSampleInventoryMenu();
      ~XXNSampleInventoryMenu();

      static constexpr char* const menuPath = "Data\\Menus\\northernui\\xxnsampleinventorymenu.xml";

   protected:
      struct ItemAndIndex {
         RE::ExtraContainerChanges::EntryData* data;
         UInt32      index;          // TODO: Do we really need this field?
         const char* name = nullptr; // cached for faster comparisons

         ItemAndIndex() {
            this->data = nullptr;
         };
         ItemAndIndex(RE::ExtraContainerChanges::EntryData* data, UInt32 index);
         ~ItemAndIndex();

         // This struct MUST have move semantics, so that using std::swap on it doesn't call its 
         // destructor and free its data early!
         //
         ItemAndIndex(ItemAndIndex&& other) {
            this->data = other.data;
            this->index = other.index;
            this->name = other.name;
            other.data = nullptr;
            other.name = nullptr;
         };
         ItemAndIndex& operator=(ItemAndIndex&& other) {
            this->data  = other.data;
            this->index = other.index;
            this->name  = other.name;
            other.data = nullptr;
            other.name = nullptr;
            return *this;
         };

         bool operator<(const ItemAndIndex& rhs) const; // just enough to allow sorting; it's lazy-alphabetical
      };
      std::vector<ItemAndIndex> inventory;

   public:
      RE::Tile* tileBackground  = nullptr;
      RE::Tile* listContainer   = nullptr;
      RE::Tile* listScrollBar   = nullptr;
      RE::Tile* listScrollThumb = nullptr;
      RE::Tile* tileItemCount   = nullptr;
      RE::Tile* tileButtonExit  = nullptr;

      enum {
         kTileID_Background,
         kTileID_ListContainer,
         kTileID_ListScrollBar,
         kTileID_ListScrollThumb,
         kTileID_ItemCount,
         kTileID_ButtonExit,
      };
      //
      enum {
         kButtonTrait_Enabled = kTileValue_user1,
      };
      enum {
         kInventoryTrait_ItemName       = kTileValue_user1,
         kInventoryTrait_ItemIcon       = kTileValue_user6,
         kInventoryTrait_ItemPocket     = kTileValue_user9,
         kInventoryTrait_ItemIsSelected = kTileValue_user10,
         kInventoryTrait_ItemRawIndex   = kTileValue_user11,
         kInventoryTrait_ItemArrayIndex = kTileValue_user14, // index within whichever item array we're rendering (i.e. all or a category)
         //kInventoryTrait_ItemIsUsable   = kTileValue_user20,
      };
      enum {
         kItemCountTrait_Count = kTileValue_user0,
      };

      virtual void	Dispose(bool); // 00 // destructor
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      //virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseUp(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleMouseout(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target);
      //virtual void	HandleFrame();
      //virtual bool	HandleKeyboardInput(char inputChar);
      virtual UInt32	GetID();
      //virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1);

      static XXNSampleInventoryMenu* Create(); // create object on the game's own memory heap

      void ClearInventory();
      void RenderInventory();
      void UpdateInventory();
};

RE::Tile* ShowXXNSampleInventoryMenuInsteadOfAlchemyMenu(RE::ExtraContainerChanges::EntryData* appa1, RE::ExtraContainerChanges::EntryData* appa2, RE::ExtraContainerChanges::EntryData* appa3, RE::ExtraContainerChanges::EntryData* appa4);