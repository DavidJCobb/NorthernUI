#include "CRC24.h"

const UInt8  _crcBitCount   = 24;
const UInt32 _crcPolynomial = 0xD3B6BA; // reversed polynomial for MSB-first
const UInt8  _crcShiftBy    = _crcBitCount - 8;

namespace cobb {
   UInt32 crc24(const char* string, size_t length) {
      UInt32 remainder = 0;
      for (size_t i = 0; i < length; i++) {
         remainder = remainder ^ (string[i] << _crcShiftBy);
         for (UInt8 j = 0; j < 8; j++) { // bits per byte
            if (remainder & 0x00800000) { // MSB set
               remainder = (remainder >> 1) ^ _crcPolynomial;
            } else {
               remainder = remainder >> 1;
            }
         }
      }
      return remainder;
   }
}