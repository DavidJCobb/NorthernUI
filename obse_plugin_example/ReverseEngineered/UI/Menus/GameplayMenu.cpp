#include "GameplayMenu.h"
#include "ReverseEngineered/INISettings.h"
#include "ReverseEngineered/UI/TagIDs.h"

namespace RE {
   void GameplayMenu::ResetDefaults() {
      {  // difficulty
         CALL_MEMBER_FN(this->tileOptDifficultySlider, UpdateFloat)(kTagID_user5, -99999.0F);
         auto a = (float*) 0x00B37B80;
         auto b = (float*) 0x00B37B70;
         CALL_MEMBER_FN(this->tileOptDifficultySlider, UpdateFloat)(kTagID_user5, *a - *b);
         CALL_MEMBER_FN(this->tileOptDifficultySlider, UpdateFloat)(kTagID_user5, 0.0F);
      }
      CALL_MEMBER_FN(this, ToggleOnOffLabel)(this->tileOptSubtitlesGeneral,  (RE::INI::Gameplay::bGeneralSubtitles->b = true));
      CALL_MEMBER_FN(this, ToggleOnOffLabel)(this->tileOptSubtitlesDialogue, (RE::INI::Gameplay::bDialogueSubtitles->b = true));
      CALL_MEMBER_FN(this, ToggleOnOffLabel)(this->tileOptCrosshair, (RE::INI::Gameplay::bCrosshair->b = true));
      CALL_MEMBER_FN(this, ToggleOnOffLabel)(this->tileOptAutosaveOnRest, (RE::INI::Gameplay::bSaveOnRest->b = true));
      CALL_MEMBER_FN(this, ToggleOnOffLabel)(this->tileOptAutosaveOnWait, (RE::INI::Gameplay::bSaveOnWait->b = true));
      CALL_MEMBER_FN(this, ToggleOnOffLabel)(this->tileOptAutosaveOnTravel, (RE::INI::Gameplay::bSaveOnTravel->b = true));
      RE::g_IniSettingCollection->SaveSetting(RE::INI::Gameplay::bGeneralSubtitles);
      RE::g_IniSettingCollection->SaveSetting(RE::INI::Gameplay::bDialogueSubtitles);
      RE::g_IniSettingCollection->SaveSetting(RE::INI::Gameplay::bCrosshair);
      RE::g_IniSettingCollection->SaveSetting(RE::INI::Gameplay::bSaveOnRest);
      RE::g_IniSettingCollection->SaveSetting(RE::INI::Gameplay::bSaveOnWait);
      RE::g_IniSettingCollection->SaveSetting(RE::INI::Gameplay::bSaveOnTravel);
   };
};