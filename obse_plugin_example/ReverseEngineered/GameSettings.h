#pragma once
#include "obse/GameAPI.h"

namespace RE {
   namespace GMST {
      extern SettingInfo* const fPotionGoldValueMult;
      //
      extern SettingInfo* const iInventoryAskQuantityAt;
      //
      extern SettingInfo* const sAddedEffects;   // "Added Effects"
      extern SettingInfo* const sAll;            // "All"
      extern SettingInfo* const sAreaText;       // "Area"
      extern SettingInfo* const sAutoAttempt;    // "Auto Attempt"
      extern SettingInfo* const sAutoLoading;    // "Autoloading..."
      extern SettingInfo* const sAutoSaving;     // "Autosaving..."
      extern SettingInfo* const sCancel;         // "Cancel"
      extern SettingInfo* const sCloseButton;    // "Close"
      extern SettingInfo* const sConfirmRecharge; // "Recharge this item for"
      extern SettingInfo* const sConfirmRepair;  // "Repair this item for"
      extern SettingInfo* const sConfirmRepairAll; // "Repair all items for"
      extern SettingInfo* const sCorruptContentMessage; // "Some of your downloaded content is corrupt and could not be loaded." // refers to DLC
      extern SettingInfo* const sCreate;         // "Create"
      extern SettingInfo* const sDeleteSuccessful; // "Save game deleted."
      extern SettingInfo* const sDurationText;   // "Duration"
      extern SettingInfo* const sFileNotFound;   // "File does not exist"
      extern SettingInfo* const sFilter;         // "Filter"
      extern SettingInfo* const sFindingContentMessage; // "Looking for content.  Please wait." // yes, two spaces // likely refers to DLC
      extern SettingInfo* const sImpossibleLock; // "This lock cannot be picked. You need a key."
      extern SettingInfo* const sKnownEffects;   // "Known Effects"
      extern SettingInfo* const sLoadingContentMessage; // "Loading extra content. Please wait." // likely refers to DLC
      extern SettingInfo* const sLockLevelText;  // "Lock Level"
      extern SettingInfo* const sMagnitude;      // "Magnitude"
      extern SettingInfo* const sMapMarkerNotFound;
      extern SettingInfo* const sMustRestart;    // "You must exit and restart Oblivion for changes in this setting to take effect."
      extern SettingInfo* const sNamePotion;     // default custom potion name
      extern SettingInfo* const sNewGame;        // "New Game"
      extern SettingInfo* const sNo;             // "No" // suitable for message box buttons
      extern SettingInfo* const sNoItemsToRecharge; // "You have no items that need to be recharged."
      extern SettingInfo* const sNoLockPick;     // "You have no lockpicks."
      extern SettingInfo* const sNoLockPickIfCrimeAlert; // "You can't pick a lock while being pursued by guards."
      extern SettingInfo* const sNoLockPickKey;  // "You need a lockpick or the key."
      extern SettingInfo* const sNoRepairMagic;  // "Only a journeyman armorer or higher may repair magic items."
      extern SettingInfo* const sNoRestart;      // "No, keep playing." // message box button for sRestartToUseNewContent
      extern SettingInfo* const sNoSoulGem;      // "You need a Soul Gem."
      extern SettingInfo* const sNoSoulInGem;    // "This Soul Gem is empty. You can only equip a Soul Gem that has a soul in it."
      extern SettingInfo* const sOffButtonText;  // for "On"/"Off" clickable buttons in the UI
      extern SettingInfo* const sOk;             // message box button text
      extern SettingInfo* const sOnButtonText;   // for "On"/"Off" clickable buttons in the UI
      extern SettingInfo* const sOpenWithKey;    // "Door opened with "
      extern SettingInfo* const sPotionSuccess;
      extern SettingInfo* const sRangeText;      // "Range"
      extern SettingInfo* const sRepairAll;      // "Repair all"
      extern SettingInfo* const sReplace;        // "Replace"
      extern SettingInfo* const sSaveFailed;     // "Save failed."
      extern SettingInfo* const sSaveSuccessful; // "Save successful."
      extern SettingInfo* const sSlotsFull;      // "You have already made the maximum number of selections."
      extern SettingInfo* const sTravelQuestion;
      extern SettingInfo* const sViewAll;        // "View All"
      extern SettingInfo* const sYes;            // "Yes" // suitable for message box buttons
      extern SettingInfo* const sYesRestart;     // "Yes, restart the game." // message box button for sRestartToUseNewContent
      //
      extern SettingInfo** const sMonthNames;   // 12 pointers
      extern SettingInfo** const sWeekdayNames; //  7 pointers
      extern SettingInfo** const sHolidayNames; // 15 pointers
      extern SettingInfo** const sSeasonNames;  //  4 pointers
      //
      extern SettingInfo** const sMagicRangeNames; // 7 pointers; includes projectile types e.g. bolt, fog
      //
      extern SettingInfo** const sControlMappingNames; // 29 pointers; names of mappings corresponding to Input.h kControl enum
   };
};