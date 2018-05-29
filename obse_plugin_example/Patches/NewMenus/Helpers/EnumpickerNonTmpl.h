#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

#include <initializer_list>
#include <string>
#include <vector>

namespace XXNMenuHelpers {
   //
   // Deliberately avoiding templating this class, because all attempts produce 
   // C2988 errors without any explanation. The compiler literally considers 
   // lines like the following malformed and I've wasted enough time trying to 
   // clean the crap off its legs:
   //
   //    template<typename T> struct Enumpicker<T> { /* ... */ }; // C2988
   //    template<typename T> struct Enumpicker    { /* ... */ }; // C2988
   //
   // The resulting code will be no less efficient. Templating just automates 
   // the process of creating copies of the class with minor variations.
   //
   struct EnumpickerUInt {
      EnumpickerUInt() {};
      EnumpickerUInt(bool sorted, std::initializer_list<UInt32> v) {
         this->sorted = sorted;
         this->values = v;
      };
      EnumpickerUInt(std::initializer_list<UInt32> v) { this->values = v; };

      UInt32    index = 0;
      RE::Tile* tileLeft = nullptr;
      RE::Tile* tileRight = nullptr;
      RE::Tile* tileValue = nullptr;
      std::vector<UInt32> values;
      bool requiresRestart = false;
      bool sorted = false; // note: only applied when Set adds an entry into the list; just sort your initializer lists by hand, friendo

      UInt32 Get() const;
      SInt32 IndexOf(const UInt32) const;
      bool HandleClick(RE::Tile* tile); // returns true if event handled; false if event not relevant
      void Render() const;
      void Set(const UInt32 value, bool addIfMissing = true);
      void Sort(); // WARNING: Doesn't update this->index !
      void Stringify(std::string& out, UInt32 index) const;
   };
   //
   struct EnumpickerStr {
      EnumpickerStr() {};
      EnumpickerStr(bool sorted) {
         this->sorted = sorted;
      };

      UInt32    index = 0;
      RE::Tile* tileLeft = nullptr;
      RE::Tile* tileRight = nullptr;
      RE::Tile* tileValue = nullptr;
      std::vector<std::string> values;
      bool requiresRestart = false;
      bool sorted = false; // note: only applied when Set adds an entry into the list; just sort your initializer lists by hand, friendo

      std::string Get() const;
      SInt32 IndexOf(const std::string&) const;
      bool HandleClick(RE::Tile* tile); // returns true if event handled; false if event not relevant
      void Render() const;
      void Set(const std::string& value, bool addIfMissing = true);
      void Sort(); // WARNING: Doesn't update this->index !
      void Stringify(std::string& out, UInt32 index) const;
   };
};