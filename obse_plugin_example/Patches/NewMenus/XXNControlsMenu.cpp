#include "XXNControlsMenu.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Menus/OptionsMenu.h"
#include "ReverseEngineered/UI/Menus/TextEditMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "Miscellaneous/strings.h"
#include "Services/INISettings.h"
#include "Services/Translations.h"
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"

#include "Patches/XboxGamepad/Main.h"
#include "Patches/XboxGamepad/CustomControls.h"

bool XXNControlsMenu::ControlUI::Assign(RE::Tile* t) {
   //auto trait = CALL_MEMBER_FN(t, GetTrait)(kTraitID_Control_Key);
   //if (!trait || !trait->bIsNum)
   //   return false;
   this->tile = t;
   //this->keyCurrent = trait->num;
   return true;
};
void XXNControlsMenu::ControlUI::Render() const {
   if (!this->tile)
      return;
   if (CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_id) != kTileID_OptionMappableControl) { // tile ID changed
      return;
   }
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Control_Key, this->keyCurrent);
   CALL_MEMBER_FN(this->tile, UpdateString)(kTraitID_Control_MappingText, RE::GMST::sControlMappingNames[this->mapping]->s);
};
void XXNControlsMenu::ControlUI::Reset() {
   this->tile = nullptr;
   this->keyCurrent = 0xFF;
   this->keyOriginal = 0xFF;
   this->mapping = 0;
};

bool XXNControlsMenu::ControlList::any_changes() const {
   for (auto it = this->begin(); it != this->end(); ++it)
      if (it->IsChanged())
         return true;
   return false;
};
bool XXNControlsMenu::ControlList::key_in_use(UInt8 key) const {
   for (auto it = this->begin(); it != this->end(); ++it)
      if (it->keyCurrent == key)
         return true;
   return false;
};
XXNControlsMenu::ControlList::iterator XXNControlsMenu::ControlList::by_key(UInt8 key) {
   auto it = this->begin();
   for (; it != this->end(); ++it)
      if (it->keyCurrent == key)
         break;
   return it;
};
XXNControlsMenu::ControlList::const_iterator XXNControlsMenu::ControlList::by_key(UInt8 key) const {
   auto it = this->cbegin();
      for (; it != this->cend(); ++it)
         if (it->keyCurrent == key)
            break;
   return it;
};
XXNControlsMenu::ControlList::iterator XXNControlsMenu::ControlList::by_mapping(UInt8 mapping) {
   auto it = this->begin();
   for (; it != this->end(); ++it)
      if (it->mapping == mapping)
         break;
   return it;
};
XXNControlsMenu::ControlList::const_iterator XXNControlsMenu::ControlList::by_mapping(UInt8 mapping) const {
   auto it = this->cbegin();
   for (; it != this->cend(); ++it)
      if (it->mapping == mapping)
         break;
   return it;
};
void XXNControlsMenu::ControlList::handle_new_tile_id(RE::Tile* t) {
   UInt8 mapping;
   {
      auto trait = CALL_MEMBER_FN(t, GetTrait)(kTraitID_Control_Mapping);
      if (!trait || !trait->bIsNum)
         return;
      mapping = trait->num;
      if (mapping > RE::kControl_MAXSTANDARD)
         return;
   }
   auto it = this->by_mapping(mapping);
   if (it == this->end()) {
      this->emplace_back(mapping);
      it = this->by_mapping(mapping);
      if (it == this->end())
         return;
   }
   if (!it->Assign(t))
      it->Reset();
};

XXNControlsMenu* XXNControlsMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNControlsMenu));
   if (memory) {
      auto result = new (memory) XXNControlsMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNControlsMenu::XXNControlsMenu() {
};
XXNControlsMenu::~XXNControlsMenu() {
};
void XXNControlsMenu::Dispose(bool free) {
   this->~XXNControlsMenu();
   if (free)
      FormHeap_Free(this);
};
void XXNControlsMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
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
      case kTileID_OptionSwapSticksGameplay_Value:
         this->optionSwapSticksGameplay.tileValue = tile;
         return;
      case kTileID_OptionSwapSticksGameplay_Left:
         this->optionSwapSticksGameplay.tileLeft = tile;
         return;
      case kTileID_OptionSwapSticksGameplay_Right:
         this->optionSwapSticksGameplay.tileRight = tile;
         return;
      case kTileID_OptionSwapSticksMenuMode_Value:
         this->optionSwapSticksMenuMode.tileValue = tile;
         return;
      case kTileID_OptionSwapSticksMenuMode_Left:
         this->optionSwapSticksMenuMode.tileLeft = tile;
         return;
      case kTileID_OptionSwapSticksMenuMode_Right:
         this->optionSwapSticksMenuMode.tileRight = tile;
         return;
      case kTileID_OptionControlSchemeName_Value:
         this->optionControlScheme.tileValue = tile;
         return;
      case kTileID_OptionControlSchemeName_Left:
         this->optionControlScheme.tileLeft = tile;
         return;
      case kTileID_OptionControlSchemeName_Right:
         this->optionControlScheme.tileRight = tile;
         return;
      case kTileID_OptionMappableControl:
         this->optionMappableControls.handle_new_tile_id(tile);
         return;
      case kTileID_OptionLookSensXSlider:
         this->optionSensitivityX.tile = tile;
         return;
      case kTileID_OptionLookSensYSlider:
         this->optionSensitivityY.tile = tile;
         return;
      case kTileID_OptionRunSensSlider:
         this->optionSensitivityRun.tile = tile;
         return;
   }
};
void XXNControlsMenu::HandleMouseUp(SInt32 tileID, RE::Tile* target) {
   if (this->IsMappingKey() || this->confirmation != kConfirmationBox_None)
      return;
   if (tileID == kTileID_ButtonExit || tileID == kTileID_ButtonSaveAndExit) {
      bool saveScheme = false;
      if (tileID == kTileID_ButtonSaveAndExit) {
         saveScheme = true;
         this->Close();
         RE::OnOptionsSubmenuClose(); // needed to bring the OptionsMenu back properly
      } else if (this->SchemeIsModified()) {
         std::string prompt;
         cobb::snprintf(prompt, NorthernUI::L10N::sControlsExitUnsaved.value, this->optionControlScheme.Get().c_str());
         ShowMessageBox(
            prompt.c_str(),
            &XXNControlsMenu::Confirm_ExitUnsaved, 0,
            NorthernUI::L10N::sControlsExitUnsavedCancel.value,
            NorthernUI::L10N::sControlsExitUnsavedSave.value,
            NorthernUI::L10N::sControlsExitUnsavedSaveAsNew.value,
            NorthernUI::L10N::sControlsExitUnsavedNoSave.value,
            nullptr
         );
         this->confirmation = kConfirmationBox_ExitUnsaved;
      } else {
         this->Close();
         RE::OnOptionsSubmenuClose(); // needed to bring the OptionsMenu back properly
      }
      this->Save(true, saveScheme); // commit all changes
      return;
   }
   if (tileID == kTileID_ButtonSaveChanges) {
      this->Save(true, true);
      return;
   }
   if (tileID == kTileID_ButtonCreateNew) {
      if (this->SchemeIsModified()) {
         std::string prompt;
         cobb::snprintf(prompt, NorthernUI::L10N::sControlsCreateUnsaved.value, this->optionControlScheme.Get().c_str());
         ShowMessageBox(
            prompt.c_str(),
            &XXNControlsMenu::Confirm_CreateUnsaved, 0,
            RE::GMST::sCancel->s,
            NorthernUI::L10N::sControlsCreateSaveFirst.value,
            NorthernUI::L10N::sControlsCreateDiscard.value,
            nullptr
         );
         this->confirmation = kConfirmationBox_CreateWhileUnsaved;
         return;
      }
      RE::ShowTextEditMenu(NorthernUI::L10N::sControlsCreateNamePrompt.value, "");
      this->modal = kModal_CreateNew;
      return;
   }
   if (tileID == kTileID_ButtonRename) {
      std::string prompt;
      auto&       value = this->optionControlScheme.Get();
      cobb::snprintf(prompt, NorthernUI::L10N::sControlsRenamePrompt.value, value.c_str());
      RE::ShowTextEditMenu(prompt.c_str(), value.c_str());
      this->modal = kModal_Rename;
      return;
   }
   if (tileID == kTileID_ButtonDelete) {
      if (this->CurrentSchemeIsDefault())
         return;
      std::string prompt;
      cobb::snprintf(prompt, NorthernUI::L10N::sControlsDelete.value, this->optionControlScheme.Get().c_str());
      ShowMessageBox(
         prompt.c_str(),
         &XXNControlsMenu::Confirm_Delete, 0,
         RE::GMST::sNo->s,
         RE::GMST::sYes->s,
         nullptr
      );
      this->confirmation = kConfirmationBox_Delete;
      return;
   }
   if (this->optionSwapSticksGameplay.HandleClick(target))
      return;
   if (this->optionSwapSticksMenuMode.HandleClick(target))
      return;
   {
      auto& option = this->optionControlScheme;
      if (target == option.tileLeft || target == option.tileRight) {
         if (this->optionMappableControls.any_changes()) {
            this->confirmation = kConfirmationBox_SwitchSchemeWithUnsaved;
            this->pendingSchemeSwitchInc = (target == option.tileRight);
            ShowMessageBox(
               NorthernUI::L10N::sControlsSwitchSchemeUnsaved.value,
               &XXNControlsMenu::Confirm_SwitchSchemeWithUnsavedChanges, 0,
               RE::GMST::sNo->s,
               RE::GMST::sYes->s,
               nullptr
            );
         } else {
            option.HandleClick(target);
            this->RenderScheme();
         }
         return;
      }
   }
   if (tileID == kTileID_OptionMappableControl) {
      this->StartRemapping(target);
      return;
   }
};
void XXNControlsMenu::HandleMouseover(SInt32 tileID, RE::Tile* target) {
};
void XXNControlsMenu::HandleMouseout(SInt32 tileID, RE::Tile* target) {
};
void XXNControlsMenu::HandleFrameMouseDown(SInt32 tileID, RE::Tile* target) {
   if (tileID == kTileID_ListScrollThumb) {
      this->HandleScrollbarThumbDrag(this->tileListScrollbar, target, RE::Menu::kScrollDragBehavior_Advanced);
      return;
   }
   if (tileID == kTileID_OptionLookSensXThumb) {
      this->HandleScrollbarThumbDrag(this->optionSensitivityX.tile, target, RE::Menu::kScrollDragBehavior_Interval, true);
      return;
   }
   if (tileID == kTileID_OptionLookSensYThumb) {
      this->HandleScrollbarThumbDrag(this->optionSensitivityY.tile, target, RE::Menu::kScrollDragBehavior_Interval, true);
      return;
   }
   if (tileID == kTileID_OptionRunSensThumb) {
      this->HandleScrollbarThumbDrag(this->optionSensitivityRun.tile, target, RE::Menu::kScrollDragBehavior_Interval, true);
      return;
   }
};
void XXNControlsMenu::HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target) {
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
   // NOTE: If list items have IDs, then we must check for those.
   //
   if (tileID == kTileID_OptionMappableControl || (tileID >= kTileID_ListPane && tileID <= kTileID_ListScrollThumb)) {
      Menu::HandleScrollbarMouseWheel(this->tileListScrollThumb);
      return;
   }
};
void XXNControlsMenu::HandleFrame() {
   if (this->modal != kModal_None) {
      auto i = RE::GetTopmostMenuIDUnderUnkCondition();
      if (i == kID) {
         auto m      = this->modal;
         this->modal = kModal_None; // clear it first so that one modal can pop another if needed
         switch (m) {
            case kModal_CreateNew:
               this->ModalCallback_CreateNew();
               break;
            case kModal_Rename:
               this->ModalCallback_Rename();
               break;
            case kModal_SaveAsNewAndExit:
               this->ModalCallback_SaveAsNewAndExit();
               break;
         }
      }
      return;
   }
   if (this->confirmation != kConfirmationBox_None)
      return;
   if (this->isAssigning == kAssignmentState_No)
      return;
   if (this->isAssigning == kAssignmentState_Starting) {
      this->isAssigning = kAssignmentState_Yes;
      return;
   }
   auto gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
   if (!gamepad) {
      this->StopRemapping(); // gamepad unplugged; boot player out of remapping attempt
      return;
   }
   SInt32 key = gamepad->AnyKeyMatches(RE::kKeyQuery_Down); // should be the same KeyQuery that the core UI support code uses to send keynav events
   if (key >= 0)
      this->StopRemapping(key);
};
UInt32 XXNControlsMenu::GetID() {
   return this->kID;
};

bool XXNControlsMenu::CurrentSchemeIsDefault() const {
   return cobb::strieq(this->optionControlScheme.Get(), NorthernUI::L10N::sControlsDefaultProfileName.value);
};
void XXNControlsMenu::RenderScheme() {
   const std::string& scheme = this->optionControlScheme.Get();
   auto& manager = XXNGamepadConfigManager::GetInstance();
   const auto profile = manager.GetProfileOrDefault(scheme);
   if (!profile) {
      _MESSAGE("XXNControlsMenu::RenderScheme: Unable to look up and render profile \"%s\".", scheme.c_str());
      //
      for (auto it = this->optionMappableControls.begin(); it != this->optionMappableControls.end(); ++it) {
         it->keyOriginal = it->keyCurrent = 0xFF;
         it->Render();
      }
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_Root_IsDefault, this->CurrentSchemeIsDefault() ? RE::kEntityID_true : RE::kEntityID_false);
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_Root_IsModified, RE::kEntityID_true);
      return;
   }
   bool any_differ = false;
   for (auto it = this->optionMappableControls.begin(); it != this->optionMappableControls.end(); ++it) {
      UInt8 mapping = it->mapping;
      it->keyOriginal = profile->bindings[mapping];
      it->keyCurrent  = profile->bindings[mapping];
      if (it->keyOriginal != it->keyCurrent)
         any_differ = true;
      it->Render();
   }
   CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_Root_IsDefault,  this->CurrentSchemeIsDefault() ? RE::kEntityID_true : RE::kEntityID_false);
   CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_Root_IsModified, any_differ ? RE::kEntityID_true : RE::kEntityID_false);
};
bool XXNControlsMenu::SchemeIsModified() const {
   return this->optionMappableControls.any_changes();
};
void XXNControlsMenu::Setup() {
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_IsMapping,     RE::kEntityID_false);
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_ControlOldKey, 255.0F);
   CALL_MEMBER_FN(this->tile, UpdateString)(kTraitID_Root_ControlName,   "");
   //
   auto& manager = XXNGamepadConfigManager::GetInstance();
   manager.GetProfileNames(this->optionControlScheme.values);
   this->optionControlScheme.Sort();
   {
      auto& current = manager.currentScheme;
      auto& option  = this->optionControlScheme;
      if (current.empty())
         this->optionControlScheme.Set(NorthernUI::L10N::sControlsDefaultProfileName.value, false);
      else
         this->optionControlScheme.Set(manager.currentScheme, false);
   }
   {  // Prefs
      this->optionSwapSticksGameplay.values.clear();
      this->optionSwapSticksGameplay.values.resize(2);
      this->optionSwapSticksGameplay.values[0] = CALL_MEMBER_FN((RE::Tile*)this->tile, GetStringTraitValue)(kTraitID_Pref_GStickSwap0);
      this->optionSwapSticksGameplay.values[1] = CALL_MEMBER_FN((RE::Tile*)this->tile, GetStringTraitValue)(kTraitID_Pref_GStickSwap1);
      this->optionSwapSticksMenuMode.values.clear();
      this->optionSwapSticksMenuMode.values.resize(2);
      this->optionSwapSticksMenuMode.values[0] = CALL_MEMBER_FN((RE::Tile*)this->tile, GetStringTraitValue)(kTraitID_Pref_MStickSwap0);
      this->optionSwapSticksMenuMode.values[1] = CALL_MEMBER_FN((RE::Tile*)this->tile, GetStringTraitValue)(kTraitID_Pref_MStickSwap1);
      this->optionSwapSticksGameplay.index = manager.swapSticksGameplay;
      this->optionSwapSticksGameplay.Render();
      this->optionSwapSticksMenuMode.index = manager.swapSticksMenuMode;
      this->optionSwapSticksMenuMode.Render();
      {  // look sensitivity
         this->optionSensitivityX.valueMin = NorthernUI::INI::XInput::fMinJoystickSensForUI.fCurrent;
         this->optionSensitivityX.valueMax = NorthernUI::INI::XInput::fMaxJoystickSensForUI.fCurrent;
         this->optionSensitivityY.valueMin = NorthernUI::INI::XInput::fMinJoystickSensForUI.fCurrent;
         this->optionSensitivityY.valueMax = NorthernUI::INI::XInput::fMaxJoystickSensForUI.fCurrent;
         this->optionSensitivityX.Set(manager.sensitivityX);
         this->optionSensitivityY.Set(manager.sensitivityY);
      }
      {  // run sensitivity
         this->optionSensitivityRun.valueMin = 0.0F;
         this->optionSensitivityRun.valueMax = 100.0F;
         this->optionSensitivityRun.Set(manager.sensitivityRun);
      }
   }
   this->RenderScheme();
};
void XXNControlsMenu::StartRemapping(RE::Tile* tile) {
   if (this->isAssigning != kAssignmentState_No)
      return;
   if (XXNGamepadSupportCore::GetInstance()->anyConnected == false) // no gamepads connected right now; can't remap
      return;
   UInt8 index = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTraitID_Control_Mapping);
   if (index > RE::kControl_COUNT)
      return;
   auto it = this->optionMappableControls.by_mapping(index);
   if (it == this->optionMappableControls.end())
      return;
   this->assigning = it;
   this->isAssigning = kAssignmentState_Starting;
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_IsMapping,     RE::kEntityID_true);
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_ControlOldKey, it->keyCurrent);
   CALL_MEMBER_FN(this->tile, UpdateString)(kTraitID_Root_ControlName,   RE::GMST::sControlMappingNames[index]->s);
};
void XXNControlsMenu::StopRemapping() {
   if (this->isAssigning == kAssignmentState_No)
      return;
   this->isAssigning = kAssignmentState_No;
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_IsMapping,     RE::kEntityID_false);
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_ControlOldKey, 255);
   CALL_MEMBER_FN(this->tile, UpdateString)(kTraitID_Root_ControlName,   "");
};
void XXNControlsMenu::StopRemapping(UInt8 key) {
   if (this->isAssigning == kAssignmentState_No)
      return;
   this->isAssigning = kAssignmentState_No;
   if (key == XXNGamepad::kGamepadButton_Start)
      key = 0xFF;
   if (key != this->assigning->keyCurrent) {
      if (this->CurrentSchemeIsDefault()) {
         //
         // If modifying the "Oblivion Default" control scheme, create a new 
         // "Untitled" control scheme as a duplicate, set us to that, and then 
         // change the key.
         //
         std::string newName;
         auto& manager = XXNGamepadConfigManager::GetInstance();
         manager.CreateNewProfile(newName);
         if (newName.empty()) { // unknown error
            CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_IsMapping, RE::kEntityID_false);
            CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_ControlOldKey, 255);
            CALL_MEMBER_FN(this->tile, UpdateString)(kTraitID_Root_ControlName, "");
            return;
         }
         this->optionControlScheme.Set(newName, true);
         CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_Root_IsDefault, RE::kEntityID_false);
      }
      auto tNew = this->assigning;
      auto tOld = this->optionMappableControls.end();
      if (key != 0xFF)
         tOld = this->optionMappableControls.by_key(key);
      if (tNew != this->optionMappableControls.end()) {
         if (tOld != this->optionMappableControls.end()) {
            //this->assigning = tNew;
            this->swapping = tOld;
            this->confirmation = kConfirmationBox_SwapKeys;
            std::string prompt;
            cobb::snprintf(prompt, NorthernUI::L10N::sControlsKeyConflict.value, RE::GMST::sControlMappingNames[tOld->mapping]->s);
            ShowMessageBox(prompt.c_str(), &XXNControlsMenu::Confirm_SwapKeys, 0, NorthernUI::L10N::sControlsKeyConflictConfirm.value, RE::GMST::sCancel->s, nullptr);
            return;
         }
         tNew->keyCurrent = key;
         tNew->Render();
      }
   }
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_IsMapping,     RE::kEntityID_false);
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_ControlOldKey, 255);
   CALL_MEMBER_FN(this->tile, UpdateString)(kTraitID_Root_ControlName,   "");
   CALL_MEMBER_FN(this->tile, UpdateFloat) (kTraitID_Root_IsModified,    this->SchemeIsModified() ? RE::kEntityID_true : RE::kEntityID_false);
};

void XXNControlsMenu::ModalCallback_CreateNew() {
   auto entry = RE::g_textEditResult->m_data;
   if (entry == nullptr) // user canceled
      return;
   std::string name = entry;
   cobb::trim(name);
   if (name.empty())
      return;
   auto& manager = XXNGamepadConfigManager::GetInstance();
   if (!manager.CreateNewProfileByName(name)) {
      std::string prompt;
      cobb::snprintf(prompt, NorthernUI::L10N::sControlsCreateNameTaken.value, name.c_str());
      ShowMessageBox(
         prompt.c_str(),
         &XXNControlsMenu::Warning_InvalidCreate, 0,
         RE::GMST::sOk->s,
         nullptr
      );
      this->confirmation = kConfirmationBox_CreateFailed;
      return;
   }
   this->optionControlScheme.Set(name, true);
   this->Save(true, true);
};
void XXNControlsMenu::ModalCallback_Rename() {
   auto entry = RE::g_textEditResult->m_data;
   if (entry == nullptr) // user canceled
      return;
   std::string rename = entry;
   cobb::trim(rename);
   if (rename.empty()) {
      ShowMessageBox(
         NorthernUI::L10N::sControlsNameCannotBeBlank.value,
         &XXNControlsMenu::Warning_InvalidRename, 0,
         RE::GMST::sOk->s,
         nullptr
      );
      this->confirmation = kConfirmationBox_RenameFailed;
      return;
   }
   if (cobb::strieq(rename, this->optionControlScheme.Get())) // user renamed it to the same value it already is
      return;
   auto& manager = XXNGamepadConfigManager::GetInstance();
   if (manager.GetProfileOrDefault(rename)) {
      std::string prompt;
      cobb::snprintf(prompt, NorthernUI::L10N::sControlsNameIsTaken.value, rename.c_str());
      ShowMessageBox(
         prompt.c_str(),
         &XXNControlsMenu::Warning_InvalidRename, 0,
         RE::GMST::sOk->s,
         nullptr
      );
      this->confirmation = kConfirmationBox_RenameFailed;
      return;
   }
   //
   // Carry out the rename:
   //
   manager.RenameScheme(this->optionControlScheme.Get(), rename);
   this->Save(true, false);
   manager.GetProfileNames(this->optionControlScheme.values);
   this->optionControlScheme.Sort();
   this->optionControlScheme.Set(rename, false);
   this->RenderScheme();
};
void XXNControlsMenu::ModalCallback_SaveAsNewAndExit() {
   auto entry = RE::g_textEditResult->m_data;
   if (entry == nullptr) // user canceled
      return;
   std::string rename = entry;
   cobb::trim(rename);
   if (rename.empty()) // user canceled or submitted an empty string
      return;
   auto& manager = XXNGamepadConfigManager::GetInstance();
   if (!manager.CreateNewProfileByName(rename)) {
      std::string  prompt;
      std::string& existingName = this->optionControlScheme.Get();
      if (cobb::strieq(rename, existingName.c_str())) {
         cobb::snprintf(prompt, NorthernUI::L10N::sControlsSaveAsNewButItsTheSameName.value, existingName.c_str());
      } else {
         cobb::snprintf(prompt, NorthernUI::L10N::sControlsExitSaveAsNewNameTaken.value, rename.c_str(), existingName.c_str());
      }
      ShowMessageBox(
         prompt.c_str(),
         &XXNControlsMenu::Confirm_ExitUnsaved, 0,
         NorthernUI::L10N::sControlsExitUnsavedCancel.value,
         NorthernUI::L10N::sControlsExitUnsavedSave.value,
         NorthernUI::L10N::sControlsExitUnsavedSaveAsNew.value,
         NorthernUI::L10N::sControlsExitUnsavedNoSave.value,
         nullptr
      );
      this->confirmation = kConfirmationBox_ExitUnsaved;
      return;
   }
   this->optionControlScheme.Set(rename, true);
   this->Save(true, true);
   //
   this->Close();
   RE::OnOptionsSubmenuClose(); // needed to bring the OptionsMenu back properly
};

/*static*/ void XXNControlsMenu::Confirm_CreateUnsaved() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*)m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   if (button == 0)
      return;
   else if (button == 1)
      menu->Save(true, true);
   //
   RE::ShowTextEditMenu(NorthernUI::L10N::sControlsCreateNamePrompt.value, "");
   menu->modal = kModal_CreateNew;
};
/*static*/ void XXNControlsMenu::Confirm_Delete() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*)m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   if (button == 1) { // deletion confirmed
      auto& manager = XXNGamepadConfigManager::GetInstance();
      manager.DeleteProfile(menu->optionControlScheme.Get());
      menu->Setup();
      menu->Save(true, false);
      CALL_MEMBER_FN(menu->tile, UpdateFloat)(kTraitID_Root_IsModified, RE::kEntityID_false);
   }
};
/*static*/ void XXNControlsMenu::Confirm_ExitUnsaved() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*)m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   if (button == 0) // Cancel
      return;
   if (button == 1) { // Save and Exit
      menu->Save(true, true);
      menu->Close();
      RE::OnOptionsSubmenuClose(); // needed to bring the OptionsMenu back properly
      return;
   }
   if (button == 2) { // Save as New, and Exit
      //ShowMessageBox(NorthernUI::L10N::sNotImplemented.value, nullptr, 0, RE::GMST::sOk->s, nullptr);
      RE::ShowTextEditMenu(NorthernUI::L10N::sControlsCreateNamePrompt.value, "");
      menu->modal = kModal_SaveAsNewAndExit;
      return;
   }
   if (button == 3) { // Exit Without Saving
      menu->Close();
      RE::OnOptionsSubmenuClose(); // needed to bring the OptionsMenu back properly
      return;
   }
};
/*static*/ void XXNControlsMenu::Confirm_SwapKeys() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*) m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   if (button == 0) { // yes, swap the keys
      UInt8 keyA = menu->assigning->keyCurrent;
      UInt8 keyS = menu->swapping->keyCurrent;
      menu->assigning->keyCurrent = keyS;
      menu->swapping->keyCurrent = keyA;
      menu->assigning->Render();
      menu->swapping->Render();
   }
   CALL_MEMBER_FN(menu->tile, UpdateFloat) (kTraitID_Root_IsMapping,     RE::kEntityID_false);
   CALL_MEMBER_FN(menu->tile, UpdateFloat) (kTraitID_Root_ControlOldKey, 255);
   CALL_MEMBER_FN(menu->tile, UpdateString)(kTraitID_Root_ControlName,   "");
   CALL_MEMBER_FN(menu->tile, UpdateFloat) (kTraitID_Root_IsModified,    RE::kEntityID_true);
   menu->confirmation = kConfirmationBox_None;
   //
   auto tile = menu->assigning->tile;
   if (tile) {
      //
      // FIX: When MessageMenu closes, the mapping tiles are all hidden (per our XML-side design). 
      // Just manually send mouseover focus back to the relevant one.
      //
      CALL_MEMBER_FN(RE::InterfaceManager::GetInstance(), SendKeynavEventTo)(tile, RE::kTagID_mouseover, 0);
   }
};
/*static*/ void XXNControlsMenu::Confirm_SwitchSchemeWithUnsavedChanges() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*) m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   if (button == 1) {
      auto t = menu->optionControlScheme.tileLeft;
      if (menu->pendingSchemeSwitchInc)
         t = menu->optionControlScheme.tileRight;
      menu->optionControlScheme.HandleClick(t);
      menu->RenderScheme();
   }
};
/*static*/ void XXNControlsMenu::Warning_InvalidCreate() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*)m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   //
   RE::ShowTextEditMenu(NorthernUI::L10N::sControlsCreateNamePrompt.value, "");
   menu->modal = kModal_CreateNew;
};
/*static*/ void XXNControlsMenu::Warning_InvalidRename() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*) m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   //
   std::string prompt;
   auto        defaultText = menu->optionControlScheme.Get().c_str();
   cobb::snprintf(prompt, NorthernUI::L10N::sControlsRenamePrompt.value, defaultText);
   RE::ShowTextEditMenu(prompt.c_str(), defaultText);
   menu->modal = kModal_Rename;
};
/*static*/ void XXNControlsMenu::Warning_InvalidSaveAsNewName() {
   auto m = (RE::TileMenu*) g_TileMenuArray->data[XXNControlsMenu::kID - kMenuType_Message];
   if (!m)
      return;
   auto menu = (XXNControlsMenu*) m->menu;
   if (!menu)
      return;
   int button = RE::GetAndResetMessageButtonPressed();
   menu->confirmation = kConfirmationBox_None;
   //
   RE::ShowTextEditMenu(NorthernUI::L10N::sControlsCreateNamePrompt.value, "");
   menu->modal = kModal_SaveAsNewAndExit;
};

void XXNControlsMenu::Save(bool prefs, bool scheme) {
   auto& manager = XXNGamepadConfigManager::GetInstance();
   if (prefs) {
      manager.swapSticksGameplay = this->optionSwapSticksGameplay.index;
      manager.swapSticksMenuMode = this->optionSwapSticksMenuMode.index;
      {  // look sensitivity
         float f;
         f = this->optionSensitivityX.Get();
         if (f)
            manager.sensitivityX = f;
         //
         f = this->optionSensitivityY.Get();
         if (f)
            manager.sensitivityY = f;
      }
      manager.sensitivityRun = this->optionSensitivityRun.Get();
   }
   if (prefs || scheme) {
      auto name = this->optionControlScheme.Get();
      auto profile = manager.GetProfile(name);
      if (!profile) {
         if (cobb::strieq(name, NorthernUI::L10N::sControlsDefaultProfileName.value))
            manager.SetProfile("");
      } else {
         manager.SetProfile(name);
         if (scheme) {
            for (auto it = this->optionMappableControls.begin(); it != this->optionMappableControls.end(); ++it) {
               profile->bindings[it->mapping] = it->keyCurrent;
               it->keyOriginal = it->keyCurrent;
            }
            CALL_MEMBER_FN(this->tile, UpdateFloat)(kTraitID_Root_IsModified, RE::kEntityID_false);
         }
      }
   }
   manager.SaveCustomProfiles();
   manager.ApplySelectedProfile();
}

RE::Tile* ShowXXNControlsMenu() {
   if (auto existing = GetMenuByType(XXNControlsMenu::kID)) {
      ((XXNControlsMenu*)existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)("Data\\Menus\\northernui\\xxncontrolsmenu.xml");
   auto menu = (XXNControlsMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu)
      return nullptr;
   if (menu->GetID() != XXNControlsMenu::kID) {
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