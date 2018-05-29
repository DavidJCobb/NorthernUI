#include "CRC32.h"

const UInt8  _crcBitCount = 32;
const UInt32 _crcPolynomial = 0xEDB88320; // reversed polynomial for MSB-first
UInt32 CRC32(char* string, size_t length) {
   UInt32 remainder = 0;
   for (size_t i = 0; i < length; i++) {
      remainder = remainder ^ (string[i] << (_crcBitCount - 8));
      for (UInt8 j = 0; j < 8; j++) { // bits per byte
         if (remainder & 0x80000000) { // MSB set
            remainder = (remainder >> 1) ^ _crcPolynomial;
         } else {
            remainder = remainder >> 1;
         }
      }
   }
   return remainder;
}