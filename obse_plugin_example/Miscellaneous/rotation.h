#pragma once

namespace cobb {
   struct quaternion;

   struct matrix {
      float data[3][3];

      float element(int index) const {
         int x = index % 3;
         int y = (index - x) / 3;
         return this->data[y][x];
      }

      float trace() const;

      explicit operator quaternion() const;
   };
   struct quaternion {
      float w = 0.0F;
      float x = 0.0F;
      float y = 0.0F;
      float z = 0.0F;

      inline float& operator[](int i) {
         if (i == 0)
            return x;
         if (i == 1)
            return y;
         if (i == 2)
            return z;
      };

      explicit operator matrix() const;
   };
};