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

#include <forward_list>
#include <unordered_map>
#include <vector>

// To create it on the game's own heap, use auto menu = XXNAlchemyMenu::Create();
// Bear in mind that some sub-objects e.g. vectors are created on our heap
// We'll probably end up writing manual construct/destruct code as we test
//
class XXNAlchemyMenu : public RE::Menu {
   public:
      enum { kID = 0x410 };
      XXNAlchemyMenu();
      ~XXNAlchemyMenu();

      static constexpr char* const menuPath = "Data\\Menus\\northernui\\xxnalchemymenu.xml";
      static constexpr bool allowNonOverlappingEffectPairs = true;

      enum {
         kTileID_Background = 0,
         kTileID_Apparatus1 = 1, // These four IDs must be contiguous; code in HandleTileIDChange uses >= and <= operators
         kTileID_Apparatus2, // 2
         kTileID_Apparatus3, // 3
         kTileID_Apparatus4, // 4
         kTileID_ItemStats          = 8,
         kTileID_FocusBox           = 9,
         kTileID_ButtonExit         = 10,
         kTileID_ButtonCreatePotion = 11,
         kTileID_ButtonDeselectAll  = 12,
         kTileID_EffectListBackground  = 15, // No special functionality except that mouse wheel events over this ID affect the scrollbar
         kTileID_EffectListContainer   = 16, // These four IDs must be contiguous; code in the wheel handler uses >= and <= operators
         kTileID_EffectListScrollbar   = 17,
         kTileID_EffectListScrollThumb = 18,
         kTileID_IngredientListBackground  = 20, // No special functionality except that mouse wheel events over this ID affect the scrollbar
         kTileID_IngredientListContainer   = 21, // These four IDs must be contiguous; code in the wheel handler uses >= and <= operators
         kTileID_IngredientListScrollbar   = 22,
         kTileID_IngredientListScrollThumb = 23,
         kTileID_NameTextboxBackground = 30,
         kTileID_NameTextboxText       = 31,
         kTileID_PotionEffectListBackground  = 40,
         kTileID_PotionEffectListContainer   = 41,
         kTileID_PotionEffectListScrollbar   = 42,
         kTileID_PotionEffectListScrollThumb = 43,
         kTileID_IngredientWasteWarning      = 44,
         kTileID_InventoryCategory          = 100, // generated tiles only
         kTileID_InventoryShowAllCategories = 101, // generated tiles only
         kTileID_InventoryItem              = 102, // generated tiles only
      };
      enum {
         kInventoryTrait_ItemName       = kTileValue_user1,
         kInventoryTrait_ItemIcon       = kTileValue_user6,
         kInventoryTrait_ItemCount      = kTileValue_user7,
         kInventoryTrait_ItemIsSelected = kTileValue_user10,
         kInventoryTrait_ItemRawIndex   = kTileValue_user11, // index within the list of all ingredients, even if we're filtering by effect
         kInventoryTrait_ItemArrayIndex = kTileValue_user14, // index within whichever item array we're rendering (i.e. all or a category)
         kInventoryTrait_ItemIsUsable   = kTileValue_user20,
      };
      enum {
         kInventoryCategoryTrait_Name       = kTileValue_string,
         kInventoryCategoryTrait_IsSelected = kTileValue_user0,
         kInventoryCategoryTrait_IsUsable   = kTileValue_user1, // you have more than one (usable) ingredient in this category, or can cook single-effect potions as a master
         kInventoryCategoryTrait_EffectCode = kTileValue_user2,
         kInventoryCategoryTrait_AVOrOther  = kTileValue_user3, // let's hope EffectItem::actorValueOrOther can always serialize to a float without precision loss
      };
      enum {
         kPotionEffectTrait_Description = kTileValue_user0,
         kPotionEffectTrait_IconPath    = kTileValue_user1,
      };
      enum {
         kItemStatsTrait_PotionWeight = kTileValue_user0,
         kItemStatsTrait_PotionValue  = kTileValue_user1,
      };
      enum {
         kWasteWarningTrait_All = kTileValue_user0,
         kWasteWarningTrait_1   = kTileValue_user1,
         kWasteWarningTrait_2   = kTileValue_user2,
         kWasteWarningTrait_ShouldShow = kTileValue_user5,
      };

   protected:
      //
      // Effect codes are UInt32s whose values indicate the general result of an EffectItem being applied; for 
      // example, 'REAT' is short for "Restore Attribute." Note that effect codes themselves are not enough to 
      // uniquely identify an alchemy effect; one must also check the actor value (e.g. 'REAT' on actor value 
      // 0x01 (Intelligence) is a "Restore Intelligence" effect specifically).
      //
      typedef UInt64 EffectCodeAndAV;
      static EffectCodeAndAV GetCombinedIdentifier(RE::EffectItem* item) {
         UInt64 out = item->effectCode;
         out |= (((UInt64) item->actorValueOrOther) << 32);
         return out;
      };

      struct EffectAndName {
         EffectCodeAndAV code;
         std::string     name;

         EffectAndName(EffectCodeAndAV a, char* b) : code(a), name(b) {};

         // This struct MUST have move semantics, so that using std::swap on it doesn't call its 
         // destructor and free its data early!
         //
         EffectAndName(EffectAndName&& other) noexcept { // move constructor
            this->code = other.code;
            this->name.swap(other.name);
            other.code = 0;
         };
         EffectAndName& operator=(EffectAndName&& other) noexcept { // move assignment
            this->code = other.code;
            this->name.swap(other.name);
            other.code = 0;
            return *this;
         };

         // Operators: less-than allows the use of std::sort to sort a list of these alphabetically; 
         // equality operators allow the use of std::find based solely on the effect code and not on 
         // the name pointer.
         //
         bool operator<(const EffectAndName& rhs) const {
            return this->name < rhs.name;
         }
         /*inline bool operator==(const EffectAndName &other) const {
            return this->code == other.code;
         }
         inline bool operator!=(const EffectAndName &other) const {
            return !(*this == other);
         }
         inline bool operator==(const EffectCodeAndAV &code) const {
            return this->code == code;
         }
         inline bool operator!=(const EffectCodeAndAV &code) const {
            return !(*this == code);
         }*/
      };
      struct ItemAndIndex {
         //
         // This struct is used to keep track of the (relevant) items in the player's inventory. 
         // It is stored in multiple places:
         //
         //  - The list of all items in the player's inventory (unfiltered list)
         //  - The lists of items matching each magic effect (filtered lists)
         //
         // The items themselves are described by the (data) field, which points to an object re-
         // trieved from the game engine. This object is a temporary copy meant for our use, and 
         // must be destroyed when the menu closes. The object may also be held by the menu's 
         // (selectedIngredients) pointers, so those should be nulled when the object is destroyed.
         //
         // The (index) field exists so that when we're looping over a filtered list, we can tell 
         // where the item is in the unfiltered list. When we render items in the GUI, they are 
         // given this index; when an item is clicked on, we use the index to pull from the unfil-
         // tered list.
         //
         RE::ExtraContainerChanges::EntryData* data;
         UInt32      index;
         const char* name = nullptr; // cached for faster comparisons when sorting

         ItemAndIndex() : data(nullptr), index(0), name(nullptr) {};
         ItemAndIndex(RE::ExtraContainerChanges::EntryData* data, UInt32 index);
         ~ItemAndIndex();

         // This struct MUST have move semantics, so that using std::swap on it doesn't call its 
         // destructor and free its data early!
         //
         ItemAndIndex(ItemAndIndex&& other) noexcept { // move constructor
            this->data = other.data;
            this->index = other.index;
            this->name = other.name;
            other.data = nullptr;
            other.name = nullptr;
         };
         ItemAndIndex& operator=(ItemAndIndex&& other) noexcept { // move assignment
            this->data = other.data;
            this->index = other.index;
            this->name = other.name;
            other.data = nullptr;
            other.name = nullptr;
            return *this;
         };

         bool operator<(const ItemAndIndex& rhs) const; // just enough to allow sorting; it's lazy-alphabetical
      };

      typedef std::vector<ItemAndIndex> ItemList;
      std::vector<EffectAndName> availableEffects; // all known effects among all ingredients in the player's inventory
      std::vector<ItemAndIndex>  ingredients;
      std::unordered_map<EffectCodeAndAV, ItemList> ingredientsByEffect; // ItemLists are subsets of (ingredients), i.e. with copies of the ItemAndIndex instances
      
   public:
      struct UIList {
         RE::Tile* container   = nullptr;
         RE::Tile* scrollBar   = nullptr;
         RE::Tile* scrollThumb = nullptr;
      };

      //
      // Expected templates:
      //  - alch_effect_list_item     == An entry in the list of available effects
      //  - alch_ingredient_list_item == An entry in the list of ingredients for a selected effect
      //

      RE::Tile* tileBackground = nullptr;
      UIList    effectList;       // effects you can choose from, including "All"
      UIList    ingredientList;   // ingredients in the selected effect
      UIList    potionEffectList; // effects for the current working potion
      RE::Tile* tileApparati[4]        = { nullptr, nullptr, nullptr, nullptr }; // apparatus tiles (child elements can show the icon, etc.)
      RE::Tile* tileItemStats          = nullptr; // ingredient name and quality; weight; value; all four effects
      RE::Tile* tilePotionInfo         = nullptr; // information on the potion being created
      RE::Tile* tileNameBackground     = nullptr;
      RE::Tile* tileNameText           = nullptr;
      RE::Tile* tileButtonCreatePotion = nullptr;
      RE::Tile* tileButtonDeselectAll  = nullptr;
      RE::Tile* tileButtonExit         = nullptr;
      RE::Tile* tileFocusBox           = nullptr;
      RE::Tile* tileWasteWarning       = nullptr;
      //
      enum TextFormState : UInt8 {
         kTextFormState_UpToDate       = 0,
         kTextFormState_ChangesPending = 1,

         kTextboxState_Empty     = 0, // textbox is empty or value is invalid
         kTextboxState_Auto      = 1, // we are managing the textbox's value
         kTextboxState_Custom    = 2, // user has changed the textbox value somehow
         kTextboxState_NewPotion = 3, // a new potion has been crafted; reset to Auto if ingredients change
      };
      //
      EffectCodeAndAV  selectedEffect = 0; // D0
      RE::ExtraContainerChanges::EntryData* selectedIngredients[4]; // pointers to entries within the ingredient list
      RE::ExtraContainerChanges::EntryData* apparati[4]; // Mortar and Pestle; ?; ?; ?
      //EffectEntry      activeEffects;
      std::forward_list<RE::EffectItem*> activeEffects; // all effects among all selected ingredients
      RE::AlchemyItem* potion = nullptr; // current working potion
      RE::MenuTextInputState* textInputState = nullptr;
      UInt8            knownEffectCount = 1;
      TextFormState    potionNameState = kTextFormState_UpToDate;

      virtual void	Dispose(bool); // 00 // destructor
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      //virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseUp(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseout(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrame();
      virtual bool	HandleKeyboardInput(UInt32 inputChar);
      virtual UInt32	GetID();
      //virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1);

      static XXNAlchemyMenu* Create(); // create object on the game's own memory heap

      void ClearInventory();
      void DeselectAll();
      void UpdateInventory();
      void UpdateSkill();
      void UpdatePotion(); // also updates user1 trait for "Create" and "Deselect All" buttons
      void CreatePotion();

      EffectCodeAndAV GetEffectAndAVFromListItem(RE::Tile* listItem) const;
      RE::ExtraContainerChanges::EntryData* GetIngredientFromListItem(RE::Tile* listItem) const;

      bool IngredientHasEffect(IngredientItem* ingredient, UInt32 effectCode);
      bool IngredientIsCompatible(IngredientItem* ingredient);
      void RenderEffectList();
      void RenderInventory();
      void UpdateRenderedEffectList();
      void UpdateRenderedInventory(); // updates list without rebuilding it, so that ingredients incompatible with our selections are shown in grey instead of white
      void UpdateTextField();
      void UpdateWasteWarning(bool wasted[4]);
};

//
// TODO:
//  - Model this on ShowAlchemyMenu
//  - Note that 0057CC00 is called immediately before ShowAlchemyMenu; investigate that as well
//     - We'll just be patching over ShowAlchemyMenu's sole caller, so we don't need to mimic 0057CC00.
//
RE::Tile* ShowXXNAlchemyMenu(RE::ExtraContainerChanges::EntryData* appa1, RE::ExtraContainerChanges::EntryData* appa2, RE::ExtraContainerChanges::EntryData* appa3, RE::ExtraContainerChanges::EntryData* appa4);