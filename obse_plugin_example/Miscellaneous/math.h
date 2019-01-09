#pragma once

namespace cobb {
   constexpr float pi = 3.1415926535897932384626433F;

   float radians_to_degrees(float rad);
   float degrees_to_radians(float deg);

   template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
   inline T distance(T x, T y) {
      return sqrt((x*x) + (y*y));
   }

   template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
   inline T range_to_range(T value, T aMin, T aMax, T bMin, T bMax) { // map a number from the A range to the B range
      return bMin + ((bMax - bMin) / (aMax - aMin)) * (value - aMin);
   }

   constexpr int32_t ceil(float num) {
      return (static_cast<float>(static_cast<int32_t>(num)) == num)
         ? static_cast<int32_t>(num)
         : static_cast<int32_t>(num) + ((num > 0) ? 1 : 0);
   }
};