#include "debug.h"

namespace cobb {
   void verify_heap(const char* message) {
      _MESSAGE("Testing heap: %s", message);
      {
         struct Test {
            UInt32 test;
         };
         auto dummy = new Test;
         dummy->test = 3;
         delete dummy; // likely to crash if the heap is corrupt
         dummy = nullptr;
      }
      _MESSAGE("Heap looks good.");
   }
}