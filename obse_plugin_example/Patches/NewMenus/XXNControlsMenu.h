#pragma once
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

#include "Helpers/Checkbox.h"
using namespace XXNMenuHelpers;
#include "Helpers/EnumpickerNonTmpl.h"
using namespace XXNMenuHelpers;
#include "Helpers/Slider.h"
using namespace XXNMenuHelpers;

class XXNControlsMenu : public RE::Menu {
   private:
      struct ControlUI {
         ControlUI(UInt8 m) : mapping(m) {};

         UInt8     mapping;
         UInt8     keyCurrent  = 0xFF;
         UInt8     keyOriginal = 0xFF;
         RE::Tile* tile = nullptr;

         bool Assign(RE::Tile*); // returns false if invalid
         inline bool IsChanged() const {
            return this->keyCurrent != this->keyOriginal;
         };
         void Render() const;
         void Reset();
         void Set(UInt8 key);
      };
      class ControlList : public std::vector<ControlUI> {
         public:
            bool any_changes() const;
            bool key_in_use(UInt8 key) const;
            iterator by_mapping(UInt8 key);
            const_iterator by_mapping(UInt8 key) const;
            iterator by_key(UInt8 key);
            const_iterator by_key(UInt8 key) const;
            void handle_new_tile_id(RE::Tile* t);
      };

   public:
      enum { kID = 0x41F };
      XXNControlsMenu();
      ~XXNControlsMenu();

      enum {
         kTileID_ButtonExit      = 1,
         kTileID_ListPane        = 2,
         kTileID_ListScrollbar   = 3,
         kTileID_ListScrollThumb = 4,
         kTileID_ButtonSaveAndExit = 5,
         kTileID_OptionSwapSticksGameplay_Value = 6,
         kTileID_OptionSwapSticksGameplay_Left = 7,
         kTileID_OptionSwapSticksGameplay_Right = 8,
         kTileID_OptionSwapSticksMenuMode_Value = 9,
         kTileID_OptionSwapSticksMenuMode_Left = 10,
         kTileID_OptionSwapSticksMenuMode_Right = 11,
         kTileID_OptionControlSchemeName_Value = 12,
         kTileID_OptionControlSchemeName_Left = 13,
         kTileID_OptionControlSchemeName_Right = 14,
         kTileID_OptionMappableControl = 15, // same ID for all controls; use traits to distinguish
         kTileID_OptionLookSensXSlider = 16,
         kTileID_OptionLookSensXThumb  = 17,
         kTileID_OptionLookSensYSlider = 18,
         kTileID_OptionLookSensYThumb  = 19,
         kTileID_ButtonRename      = 20, // rename current scheme
         kTileID_ButtonSaveChanges = 21, // save changes to current scheme
         kTileID_ButtonDelete      = 22, // delete current scheme
         kTileID_ButtonCreateNew   = 23, // create new scheme
         kTileID_OptionRunSensSlider = 24,
         kTileID_OptionRunSensThumb  = 25,
      };
      enum {
         kTraitID_Root_IsMapping     = kTileValue_user0, // user0 - bool
         kTraitID_Root_ControlName   = kTileValue_user1, // user1 - string identifying the game function being remapped
         kTraitID_Root_ControlOldKey = kTileValue_user2, // user2 - int identifying the gamepad keycode already mapped to the game function being remapped
         kTraitID_Root_IsDefault     = kTileValue_user3, // user3 - bool: current scheme is the default and cannot be edited or deleted
         kTraitID_Root_IsModified    = kTileValue_user4, // user4 - bool: current scheme has been modified
         kTraitID_Root_SensMin       = kTileValue_user5,
         kTraitID_Root_SensMax       = kTileValue_user6,
         //
         kTraitID_Control_Mapping    = kTileValue_user0, // user0 - int identifying the game function
         kTraitID_Control_Key        = kTileValue_user1, // user1 - int identifying the gamepad keycode
         kTraitID_Control_MappingText = kTileValue_string, // string - string for the game function's name
         //
         kTraitID_Pref_GStickSwap0   = kTileValue_user12,
         kTraitID_Pref_GStickSwap1   = kTileValue_user13,
         kTraitID_Pref_MStickSwap0   = kTileValue_user14,
         kTraitID_Pref_MStickSwap1   = kTileValue_user15,
      };
      enum : UInt8 {
         kAssignmentState_No = 0, // NOTE: This is also set when waiting for the "Swap Keys?" confirmation dialog to be answered.
         kAssignmentState_Starting = 1, // HandleFrame needs to ignore the first frame, because it runs on the same frame as HandleMouseUp and would catch the keypress that triggered assignment
         kAssignmentState_Yes = 2,
      };
      enum ConfirmationBox : UInt8 {
         kConfirmationBox_None = 0,
         kConfirmationBox_CreateFailed,
         kConfirmationBox_CreateWhileUnsaved,
         kConfirmationBox_Delete,
         kConfirmationBox_ExitUnsaved,
         kConfirmationBox_RenameFailed,
         kConfirmationBox_SwapKeys,
         kConfirmationBox_SwitchSchemeWithUnsaved,
      };
      enum Modal : UInt8 {
         kModal_None = 0,
         kModal_CreateNew,
         kModal_Rename,
         kModal_SaveAsNewAndExit,
      };

      static constexpr float ce_lookSensitivitySliderOffset = 1.0F;

      RE::Tile* tileButtonExit      = nullptr;
      RE::Tile* tileListPane        = nullptr;
      RE::Tile* tileListScrollbar   = nullptr;
      RE::Tile* tileListScrollThumb = nullptr;

      EnumpickerStr optionSwapSticksGameplay;
      EnumpickerStr optionSwapSticksMenuMode;
      EnumpickerStr optionControlScheme = EnumpickerStr(true);
      Slider        optionSensitivityX;
      Slider        optionSensitivityY;
      Slider        optionSensitivityRun;
      ControlList   optionMappableControls;

      ControlList::iterator assigning; // key we're currently assigning
      ControlList::iterator swapping;  // key we need to swap with
      UInt8                 isAssigning = kAssignmentState_No;

      Modal           modal        = kModal_None;
      ConfirmationBox confirmation = kConfirmationBox_None;
      bool pendingSchemeSwitchInc = false; // if true, then we're in the "switch schemes?" confirmation box, and upon accepting, we increment instead of decrement
      
      virtual void	Dispose(bool); // 00 // destructor
      virtual void	HandleTileIDChange(SInt32 newID, RE::Tile* tile);
      //virtual void	HandleMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseUp(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseover(SInt32 tileID, RE::Tile* target);
      virtual void	HandleMouseout(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseDown(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrameMouseWheel(SInt32 tileID, RE::Tile* target);
      virtual void	HandleFrame();
      //virtual bool	HandleKeyboardInput(UInt32 inputChar);
      virtual UInt32	GetID();
      //virtual bool	HandleNavigationInput(NavigationInput argEnum, float arg1);

      static XXNControlsMenu* Create(); // create object on the game's own memory heap
      static void Confirm_CreateUnsaved();
      static void Confirm_Delete();
      static void Confirm_ExitUnsaved();
      static void Confirm_SwapKeys();
      static void Confirm_SwitchSchemeWithUnsavedChanges();
      static void Warning_InvalidCreate();
      static void Warning_InvalidRename();
      static void Warning_InvalidSaveAsNewName();

      inline bool IsMappingKey() const {
         return this->isAssigning != kAssignmentState_No;
      };
      bool CurrentSchemeIsDefault() const;
      void RenderScheme();
      void Save(bool prefs, bool scheme);
      bool SchemeIsModified() const;
      void Setup(); // called when the menu opens, or when the player deletes a control scheme
      void StartRemapping(RE::Tile* binding);
      void StopRemapping(UInt8 key);
      void StopRemapping(); // for emergencies, e.g. the gamepad being unplugged
      //
      void ModalCallback_CreateNew();
      void ModalCallback_Rename();
      void ModalCallback_SaveAsNewAndExit();
};
RE::Tile* ShowXXNControlsMenu();