#include "XXNOptionsMenu.h"
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Menus/OptionsMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"
#include "Services/Translations.h"

#include <sstream>

XXNOptionsMenu* XXNOptionsMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNOptionsMenu));
   if (memory) {
      auto result = new (memory) XXNOptionsMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNOptionsMenu::XXNOptionsMenu() {
};
XXNOptionsMenu::~XXNOptionsMenu() {
};
void XXNOptionsMenu::Dispose(bool free) {
   this->~XXNOptionsMenu();
   if (free)
      FormHeap_Free(this);
};
void XXNOptionsMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
   switch (newID) {
      case kTileID_ButtonExit:
         this->tileButtonExit = tile;
         return;
      case kTileID_ListPane:
         this->tileListPane = tile;
         return;
      case kTileID_ListScrollbar:
         this->tileListScrollbar = tile;
         return;
      case kTileID_ListScrollThumb:
         this->tileListScrollThumb = tile;
         return;
      case kTileID_OptionLocalMapRes_Left:
         this->optionLocalMapRes.tileLeft = tile;
         return;
      case kTileID_OptionLocalMapRes_Right:
         this->optionLocalMapRes.tileRight = tile;
         return;
      case kTileID_OptionLocalMapRes_Value:
         this->optionLocalMapRes.tileValue = tile;
         return;
      case kTileID_OptionSuppressDLCSpam:
         this->optionSuppressDLCPopups.tile = tile;
         return;
      case kTileID_OptionUseXXNAlchemyMenu:
         this->optionUseXXNAlchemyMenu.tile = tile;
         return;
      case kTileID_OptionShowHUDClock:
         this->optionShowHUDClock.tile = tile;
         return;
      case kTileID_OptionShowHUDInputViewer:
         this->optionShowHUDInputViewer.tile = tile;
         return;
      case kTileID_OptionUseXInputIfPatched:
         this->optionUseXInputIfPatched.tile = tile;
         return;
   }
};
void XXNOptionsMenu::HandleMouseUp(SInt32 tileID, RE::Tile* target) {
   if (tileID == kTileID_ButtonExit) {
      this->Commit();
      this->Close();
      RE::OnOptionsSubmenuClose(); // needed to bring the OptionsMenu back properly
      return;
   }
   if (tileID == kTileID_ButtonRestoreDefaults) {
      this->confirmation = kConfirmationBox_ResetDefaults;
      ShowMessageBox(
         NorthernUI::L10N::sResetOptionsToDefault.value,
         &XXNOptionsMenu::Confirm_ResetDefaults, 0,
         RE::GMST::sNo->s,
         RE::GMST::sYes->s,
         nullptr
      );
      return;
   }
   if (this->optionLocalMapRes.HandleClick(target))
      return;
   if (this->optionSuppressDLCPopups.HandleClick(target))
      return;
   if (this->optionUseXXNAlchemyMenu.HandleClick(target))
      return;
   if (this->optionShowHUDClock.HandleClick(target))
      return;
   if (this->optionShowHUDInputViewer.HandleClick(target))
      return;
   if (this->optionUseXInputIfPatched.HandleClick(target))
      return;
};
void XXNOptionsMenu::HandleMouseover(SInt32 tileID, RE::Tile* target) {
};
void XXNOptionsMenu::HandleMouseout(SInt32 tileID, RE::Tile* target) {
};
void XXNOptionsMenu::HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target) {
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
   // TODO: If list items have IDs, then we must check for those.
   //
   if (tileID >= kTileID_ListPane && tileID <= kTileID_ListScrollThumb) {
      Menu::HandleScrollbarMouseWheel(this->tileListScrollThumb);
      return;
   }
};
UInt32 XXNOptionsMenu::GetID() {
   return this->kID;
};

bool XXNOptionsMenu::AnyUnchanged() const {
   if (NorthernUI::INI::Display::uLocalMapResolutionPerCell.iCurrent != this->optionLocalMapRes.Get())
      return true;
   if (NorthernUI::INI::Features::bSuppressDLCStartup.bCurrent != this->optionSuppressDLCPopups.Get())
      return true;
   if (NorthernUI::INI::Menus::bUseXXNAlchemyMenu.bCurrent != this->optionUseXXNAlchemyMenu.Get())
      return true;
   if (NorthernUI::INI::Features::bShowHUDClock.bCurrent != this->optionShowHUDClock.Get())
      return true;
   if (NorthernUI::INI::Features::bShowHUDInputViewer.bCurrent != this->optionShowHUDInputViewer.Get())
      return true;
   if (NorthernUI::INI::XInput::bDontUseEvenWhenPatched.bCurrent != !this->optionUseXInputIfPatched.Get()) // NOTE: UI shows opposite!
      return true;
   return false;
};
void XXNOptionsMenu::Commit() {
   NorthernUI::INI::Display::uLocalMapResolutionPerCell.iCurrent = this->optionLocalMapRes.Get();
   NorthernUI::INI::Features::bSuppressDLCStartup.bCurrent       = this->optionSuppressDLCPopups.Get();
   NorthernUI::INI::Menus::bUseXXNAlchemyMenu.bCurrent           = this->optionUseXXNAlchemyMenu.Get();
   NorthernUI::INI::Features::bShowHUDClock.bCurrent             = this->optionShowHUDClock.Get();
   NorthernUI::INI::Features::bShowHUDInputViewer.bCurrent       = this->optionShowHUDInputViewer.Get();
   NorthernUI::INI::XInput::bDontUseEvenWhenPatched.bCurrent     = !this->optionUseXInputIfPatched.Get(); // NOTE: UI shows opposite!
   //
   (NorthernUI::INI::INISettingManager::GetInstance()).Save();
   NorthernUI::INI::SendChangeEvent();
};
void XXNOptionsMenu::ResetDefaults() {
   this->optionLocalMapRes.Set(NorthernUI::INI::Display::uLocalMapResolutionPerCell.iDefault, true);
   this->optionSuppressDLCPopups.Set(NorthernUI::INI::Features::bSuppressDLCStartup.bDefault);
   this->optionUseXXNAlchemyMenu.Set(NorthernUI::INI::Menus::bUseXXNAlchemyMenu.bDefault);
   this->optionShowHUDClock.Set(NorthernUI::INI::Features::bShowHUDClock.bDefault);
   this->optionShowHUDInputViewer.Set(NorthernUI::INI::Features::bShowHUDInputViewer.bDefault);
   this->optionUseXInputIfPatched.Set(!NorthernUI::INI::XInput::bDontUseEvenWhenPatched.bDefault); // NOTE: UI shows opposite!
};
void XXNOptionsMenu::Setup() {
   //
   // any menu setup tasks to be performed after opening
   //
   this->optionLocalMapRes.Set(NorthernUI::INI::Display::uLocalMapResolutionPerCell.iCurrent, true);
   this->optionSuppressDLCPopups.Set(NorthernUI::INI::Features::bSuppressDLCStartup.bCurrent);
   this->optionUseXXNAlchemyMenu.Set(NorthernUI::INI::Menus::bUseXXNAlchemyMenu.bCurrent);
   this->optionShowHUDClock.Set(NorthernUI::INI::Features::bShowHUDClock.bCurrent);
   this->optionShowHUDInputViewer.Set(NorthernUI::INI::Features::bShowHUDInputViewer.bCurrent);
   this->optionUseXInputIfPatched.Set(!NorthernUI::INI::XInput::bDontUseEvenWhenPatched.bCurrent); // NOTE: UI shows opposite!
};

/*static*/ void XXNOptionsMenu::Confirm_ResetDefaults() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNOptionsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNOptionsMenu*) m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   if (button == 1) {
      menu->ResetDefaults();
   }
};

RE::Tile* ShowXXNOptionsMenu() {
   if (auto existing = GetMenuByType(XXNOptionsMenu::kID)) {
      ((XXNOptionsMenu*)existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)("Data\\Menus\\northernui\\xxnoptionsmenu.xml");
   auto menu = (XXNOptionsMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu)
      return nullptr;
   if (menu->GetID() != XXNOptionsMenu::kID) {
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
         if (stackingtype == RE::kTagID_unknown_stack_type || stackingtype == RE::kEntityID_no_click_past) {
            float depth = RE::GetNewMenuDepth();
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_depth, depth);
         }
      }
   }
   menu->Setup();
   CALL_MEMBER_FN(menu, EnableMenu)(0);
   return tile;
};