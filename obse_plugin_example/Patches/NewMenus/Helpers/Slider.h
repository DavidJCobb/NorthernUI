#pragma once
#include "ReverseEngineered/UI/Tile.h"
#include "Miscellaneous/math.h"

namespace XXNMenuHelpers {
   struct Slider {
      public:
         Slider() {};

         RE::Tile* tile = nullptr;

         inline float get_value() const {
            if (!this->tile)
               return 0.0F;
            return CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user7);
         }
         void set_value(float value) {
            if (!this->tile)
               return;
            value -= this->slider_min(); // needed if the minimum != 0
            CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user5, -999999.0F); // force the slider's current value to its minimum bound, to reset it
            CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user5, value);      // setting user5 is only accurate if the slider is at its minimum
            CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user5, 0.0F);       // once we've inserted our value, reset user5 to avoid breaking subsequent clicks
         }

         inline float slider_min() const noexcept {
            if (!this->tile)
               return 0.0F;
            return CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user1);
         };
         inline float slider_max() const noexcept {
            if (!this->tile)
               return 0.0F;
            return CALL_MEMBER_FN(this->tile, GetFloatTraitValue)(kTileValue_user2);
         };
         inline void slider_min(float v) const noexcept {
            if (this->tile)
               CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user1, v);
         };
         inline void slider_max(float v) const noexcept {
            if (this->tile)
               CALL_MEMBER_FN(this->tile, UpdateFloat)(kTileValue_user2, v);
         };
   };
   struct NormalizedSlider : public Slider {
      NormalizedSlider() {};
      NormalizedSlider(float a, float b) : normalizedMin(a), normalizedMax(b) {};
      //
      float normalizedMin = 0.0F;
      float normalizedMax = 100.0F;

      inline float normalize(float value) const { // slider range -> normalized range
         return cobb::range_to_range(value, this->slider_min(), this->slider_max(), this->normalizedMin, this->normalizedMax);
      }
      inline float denormalize(float value) const { // normalized range -> slider range
         return cobb::range_to_range(value, this->normalizedMin, this->normalizedMax, this->slider_min(), this->slider_max());
      }

      template<bool getNormalized> float get_value() const;
      template<> float get_value<false>() const {
         return Slider::get_value();
      };
      template<> float get_value<true>() const {
         return this->normalize(Slider::get_value());
      };

      template<bool valueIsNormalized> void set_value(float v);
      template<> void set_value<false>(float v) {
         Slider::set_value(v);
      };
      template<> void set_value<true>(float v) {
         Slider::set_value(this->denormalize(v));
      };
   };
};