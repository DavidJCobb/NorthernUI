#include "Slider.h"
#include "Miscellaneous/math.h"

namespace XXNMenuHelpers {
   float Slider::ToMyRange(float value) const {
      float  sliderMin = CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user1);
      float  sliderMax = CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user2);
      return cobb::range_to_range(value, sliderMin, sliderMax, this->valueMin, this->valueMax);
   };
   float Slider::ToTileRange(float value) const {
      float  sliderMin = CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user1);
      float  sliderMax = CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user2);
      return cobb::range_to_range(value, this->valueMin, this->valueMax, sliderMin, sliderMax);
   };
   float Slider::Get() const {
      if (!this->tile)
         return 0.0F;
      float value = CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user7);
      return this->ToMyRange(value);
   };
   void Slider::Set(float value) {
      if (!this->tile)
         return;
      float finalVal = this->ToTileRange(value);
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user5, -999999.0); // force the slider's current value to its minimum bound, to reset it
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user5, finalVal);  // setting user5 is only accurate if the slider is at its minimum
      CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user5, 0.0);       // once we've inserted our value, reset user5 to avoid breaking subsequent clicks
   };
   void Slider::SetBounds(float fMin, float fMax) {
      this->valueMin = fMin;
      this->valueMax = fMax;
   }
};