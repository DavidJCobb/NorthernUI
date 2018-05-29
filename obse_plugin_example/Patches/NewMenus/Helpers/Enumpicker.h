#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

#include <initializer_list>
#include <string>
#include <vector>

#include <sstream> // needed by implementation

//
// DO NOT USE THIS FILE
//
// It doesn't compile correctly. MSVC farts out C2988 errors without explanation, and no 
// amount of searching has managed to reveal any relevant causes. The classes work perfectly 
// when they aren't templated, and I've wasted more than enough time trying to get the damn 
// template working.
//
// The only difference between templating the class and just creating variants by hand is 
// in maintability -- only having to edit one templated class. That difference becomes abso-
// lutely worthless when templating ITSELF is broken and the garbage won't even compile. 
// I don't know whether it's an MSVC problem or what, but the choice here is between "easily 
// maintainable" code that isn't usable, or working code that requires you to do the compiler's 
// job for it.
//

namespace XXNMenuHelpers {
   template<typename T> struct Enumpicker {
      Enumpicker() {};
      Enumpicker(bool sorted, std::initializer_list<T> v) {
         this->sorted = sorted;
         this->values = v;
      };
      Enumpicker(std::initializer_list<T> v) { this->values = v; };

      UInt32    index = 0;
      RE::Tile* tileLeft = nullptr;
      RE::Tile* tileRight = nullptr;
      RE::Tile* tileValue = nullptr;
      std::vector<T> values;
      bool requiresRestart = false;
      bool sorted = false; // note: only applied when Set adds an entry into the list; just sort your initializer lists by hand, friendo

      T Get() const;
      SInt32 IndexOf(const T&) const;
      bool HandleClick(RE::Tile* tile); // returns true if event handled; false if event not relevant
      void Render() const;
      void Set(const T& value, bool addIfMissing = true);
      void Sort(); // WARNING: Doesn't update this->index !
      void Stringify(std::string& out, UInt32 index) const;
   };
};
#include "Enumpicker.cpp"