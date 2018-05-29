#include "math.h"

namespace cobb {
   float radians_to_degrees(float rad) {
      return rad * 180 / pi;
   }
   float degrees_to_radians(float deg) {
      return deg * pi / 180;
   }
};