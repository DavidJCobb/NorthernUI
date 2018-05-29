#include "AudioMenu.h"
#include "ReverseEngineered/UI/TagIDs.h"

namespace RE {
   void AudioMenu::ResetDefaults() {
      {  // volume, master
         CALL_MEMBER_FN(this->tileVolumeMasterSlider, UpdateFloat)(kTagID_user5, -99999.0F);
         CALL_MEMBER_FN(this->tileVolumeMasterSlider, UpdateFloat)(kTagID_user5, 100.0F);
         CALL_MEMBER_FN(this->tileVolumeMasterSlider, UpdateFloat)(kTagID_user5, 0.0F);
      }
      {  // volume, music
         CALL_MEMBER_FN(this->tileVolumeMusicSlider, UpdateFloat)(kTagID_user5, -99999.0F);
         CALL_MEMBER_FN(this->tileVolumeMusicSlider, UpdateFloat)(kTagID_user5, 30.0F);
         CALL_MEMBER_FN(this->tileVolumeMusicSlider, UpdateFloat)(kTagID_user5, 0.0F);
      }
      {  // volume, footstep
         CALL_MEMBER_FN(this->tileVolumeFootstepSlider, UpdateFloat)(kTagID_user5, -99999.0F);
         CALL_MEMBER_FN(this->tileVolumeFootstepSlider, UpdateFloat)(kTagID_user5, 80.0F);
         CALL_MEMBER_FN(this->tileVolumeFootstepSlider, UpdateFloat)(kTagID_user5, 0.0F);
      }
      {  // volume, voice
         CALL_MEMBER_FN(this->tileVolumeVoiceSlider, UpdateFloat)(kTagID_user5, -99999.0F);
         CALL_MEMBER_FN(this->tileVolumeVoiceSlider, UpdateFloat)(kTagID_user5, 80.0F);
         CALL_MEMBER_FN(this->tileVolumeVoiceSlider, UpdateFloat)(kTagID_user5, 0.0F);
      }
      {  // volume, effects
         CALL_MEMBER_FN(this->tileVolumeEffectsSlider, UpdateFloat)(kTagID_user5, -99999.0F);
         CALL_MEMBER_FN(this->tileVolumeEffectsSlider, UpdateFloat)(kTagID_user5, 80.0F);
         CALL_MEMBER_FN(this->tileVolumeEffectsSlider, UpdateFloat)(kTagID_user5, 0.0F);
      }
   };
};