#include "rotation.h"

namespace cobb {
   float matrix::trace() const {
      return data[0][0] + data[1][1] + data[2][2];
   };
   matrix::operator quaternion() const {
      quaternion q;
      //
      // Shoemake's 1987 algorithm that literally everyone uses. Source has apparently 
      // been lost to time.
      //
      float a = this->trace();
      if (a > 0) {
         a = sqrt(a + 1.0f);
         q.w = a * 0.5f;
         a = 0.5f / a;
         q.x = (this->data[2][1] - this->data[1][2]) * a;
         q.y = (this->data[0][2] - this->data[2][0]) * a;
         q.z = (this->data[1][0] - this->data[0][1]) * a;
      } else {
         UInt8 i = 0;
         if (this->data[1][1] > this->data[0][0])
            i = 1;
         if (this->data[2][2] > this->data[i][i])
            i = 2;
         UInt8 j = (i + 1) % 3;
         UInt8 k = (j + 1) % 3;
         //
         a = sqrt(this->data[i][i] - this->data[j][j] - this->data[k][k] + 1.0f);
         q[i] = a / 2; // q[n] = value, such that q[0] == x; todo: code a proper [] operator for quaternion
         a = 0.5 / a;
         q.w  = (this->data[k][j] - this->data[j][k]) * a;
         q[j] = (this->data[j][i] + this->data[i][j]) * a;
         q[k] = (this->data[k][i] + this->data[i][k]) * a;
      }
      return q;
   }
   //
   quaternion::operator matrix() const {
      matrix output;
      output.data[0][0] = 1 - 2 * pow(y, 2) - 2 * pow(z, 2);
      output.data[0][1] = (2 * x * y) - (2 * z * w);
      output.data[0][2] = (2 * x * z) + (2 * y * w);
      output.data[1][0] = (2 * x * y) + (2 * z * w);
      output.data[1][1] = 1 - 2 * pow(x, 2) - 2 * pow(z, 2);
      output.data[1][2] = (2 * y * z) - (2 * x * w);
      output.data[2][0] = (2 * x * z) - (2 * y * w);
      output.data[2][1] = (2 * y * z) + (2 * x * w);
      output.data[2][2] = 1 - 2 * pow(x, 2) - 2 * pow(y, 2);
      return output;
   }
}