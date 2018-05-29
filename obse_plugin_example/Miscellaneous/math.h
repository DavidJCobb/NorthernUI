#pragma once

namespace cobb {
   constexpr float pi = 3.1415926535897932384626433F;

   float radians_to_degrees(float rad);
   float degrees_to_radians(float deg);

   constexpr int32_t ceil(float num) {
      return (static_cast<float>(static_cast<int32_t>(num)) == num)
         ? static_cast<int32_t>(num)
         : static_cast<int32_t>(num) + ((num > 0) ? 1 : 0);
   }
};