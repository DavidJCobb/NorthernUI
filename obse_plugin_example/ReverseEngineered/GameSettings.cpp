#include "GameSettings.h"

namespace RE {
   namespace GMST {
      SettingInfo* const fPotionGoldValueMult = (SettingInfo*) 0x00B37A48;
      //
      SettingInfo* const iInventoryAskQuantityAt = (SettingInfo*) 0x00B38688;
      //
      SettingInfo* const sAddedEffects   = (SettingInfo*) 0x00B38918;
      SettingInfo* const sAll            = (SettingInfo*) 0x00B388A8;
      SettingInfo* const sAreaText       = (SettingInfo*) 0x00B38950;
      SettingInfo* const sAutoAttempt    = (SettingInfo*) 0x00B38928;
      SettingInfo* const sAutoLoading    = (SettingInfo*) 0x00B387C0;
      SettingInfo* const sAutoSaving     = (SettingInfo*) 0x00B387B8;
      SettingInfo* const sCancel         = (SettingInfo*) 0x00B38D08;
      SettingInfo* const sCloseButton    = (SettingInfo*) 0x00B38920;
      SettingInfo* const sConfirmRecharge = (SettingInfo*) 0x00B38858;
      SettingInfo* const sConfirmRepair  = (SettingInfo*) 0x00B38840;
      SettingInfo* const sConfirmRepairAll = (SettingInfo*) 0x00B38850;
      SettingInfo* const sCorruptContentMessage = (SettingInfo*) 0x00B38010;
      SettingInfo* const sCreate         = (SettingInfo*) 0x00B38648;
      SettingInfo* const sDeleteSuccessful = (SettingInfo*) 0x00B387E0;
      SettingInfo* const sDurationText   = (SettingInfo*) 0x00B38938;
      SettingInfo* const sFileNotFound   = (SettingInfo*) 0x00B387B0;
      SettingInfo* const sFilter         = (SettingInfo*) 0x00B38898;
      SettingInfo* const sFindingContentMessage = (SettingInfo*) 0x00B38838;
      SettingInfo* const sImpossibleLock = (SettingInfo*) 0x00B38690;
      SettingInfo* const sKnownEffects   = (SettingInfo*) 0x00B38910;
      SettingInfo* const sLoadingContentMessage = (SettingInfo*) 0x00B38830;
      SettingInfo* const sLockLevelText  = (SettingInfo*) 0x00B38940;
      SettingInfo* const sMagnitude      = (SettingInfo*) 0x00B38930;
      SettingInfo* const sMapMarkerNotFound = (SettingInfo*) 0x00B38C90;
      SettingInfo* const sMustRestart    = (SettingInfo*) 0x00B38CE8;
      SettingInfo* const sNamePotion     = (SettingInfo*) 0x00B38900;
      SettingInfo* const sNewGame        = (SettingInfo*) 0x00B387C8;
      SettingInfo* const sNo             = (SettingInfo*) 0x00B38D00;
      SettingInfo* const sNoItemsToRecharge = (SettingInfo*) 0x00B38878;
      SettingInfo* const sNoLockPick     = (SettingInfo*) 0x00B386A8;
      SettingInfo* const sNoLockPickIfCrimeAlert = (SettingInfo*) 0x00B386B0;
      SettingInfo* const sNoLockPickKey  = (SettingInfo*) 0x00B3869C;
      SettingInfo* const sNoRepairMagic  = (SettingInfo*) 0x00B38880;
      SettingInfo* const sNoRestart      = (SettingInfo*) 0x00B38808;
      SettingInfo* const sNoSoulGem      = (SettingInfo*) 0x00B38868;
      SettingInfo* const sNoSoulInGem    = (SettingInfo*) 0x00B38870;
      SettingInfo* const sOffButtonText  = (SettingInfo*) 0x00B38DA8;
      SettingInfo* const sOk             = (SettingInfo*) 0x00B38CF0;
      SettingInfo* const sOnButtonText   = (SettingInfo*) 0x00B38DA0;
      SettingInfo* const sOpenWithKey    = (SettingInfo*) 0x00B386A0;
      SettingInfo* const sPotionSuccess  = (SettingInfo*) 0x00B388E8;
      SettingInfo* const sRangeText      = (SettingInfo*) 0x00B38958;
      SettingInfo* const sRepairAll      = (SettingInfo*) 0x00B38848;
      SettingInfo* const sReplace        = (SettingInfo*) 0x00B38650;
      SettingInfo* const sSaveFailed     = (SettingInfo*) 0x00B387D8;
      SettingInfo* const sSaveSuccessful = (SettingInfo*) 0x00B387D0;
      SettingInfo* const sSlotsFull      = (SettingInfo*) 0x00B38620;
      SettingInfo* const sTravelQuestion = (SettingInfo*) 0x00B38C60;
      SettingInfo* const sViewAll        = (SettingInfo*) 0x00B388A0;
      SettingInfo* const sYes            = (SettingInfo*) 0x00B38CF8;
      SettingInfo* const sYesRestart     = (SettingInfo*) 0x00B38800;
      //
      SettingInfo** const sMonthNames   = (SettingInfo**) 0x00B06FA4; // twelve pointers
      SettingInfo** const sWeekdayNames = (SettingInfo**) 0x00B06FD4; // seven pointers
      SettingInfo** const sHolidayNames = (SettingInfo**) 0x00B06FF0; // fifteen pointers
      SettingInfo** const sSeasonNames  = (SettingInfo**) 0x00B0702C; // four pointers
      //
      SettingInfo** const sMagicRangeNames = (SettingInfo**) 0x00B037E4; // seven pointers
      //
      SettingInfo** const sControlMappingNames = (SettingInfo**) 0x00B399D0; // 29 pointers; names of mappings corresponding to Input.h kControl enum
      //
      SettingInfo** const sReticleVerbs = (SettingInfo**) 0x00B39A64;
   };
};