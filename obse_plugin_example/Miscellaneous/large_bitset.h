#pragma once
#include <bitset>

//
// Allow the use of an initializer_list of bit indices to set the initial 
// value, so that large bitsets can be initialized without using a string.
//

namespace cobb {
   template<size_t _Bits>
   class large_bitset : public std::bitset<_Bits> {
      public:
         //using bitset::bitset<_Bits>; // Inherit constructors automatically // doesn't compile; unsupported?
         // Inherit constructors manually:
         constexpr large_bitset() _NOEXCEPT : bitset() {}; // initialize to all-false
         constexpr large_bitset(_ULonglong _Val) _NOEXCEPT : bitset<_Bits>(_Val) {}; // construct from bits in int
         //*/
         large_bitset(std::initializer_list<size_t> bitIndices) { // initialize from initializer list
            for (auto it = bitIndices.begin(); it != bitIndices.end(); ++it)
               this->set(*it);
         };
         large_bitset(std::vector<size_t> bitIndices) { // initialize from vector
            for (auto it = bitIndices.begin(); it != bitIndices.end(); ++it)
               this->set(*it);
         };
   };
};