#include "EnumpickerNonTmpl.h"

#include "Miscellaneous/strings.h"
#include <sstream>

namespace XXNMenuHelpers {

   UInt32 EnumpickerUInt::Get() const {
      if (!this->values.size())
         return 0;
      return this->values[this->index];
   };
   bool EnumpickerUInt::HandleClick(RE::Tile* tile) {
      auto s = this->values.size();
      if (tile == this->tileLeft) {
         this->index--; // if this goes below zero, it'll underflow
         if (this->index >= s)
            this->index = s - 1;
      } else if (tile == this->tileRight) {
         this->index++;
         if (this->index >= s)
            this->index = 0;
      } else
         return false;
      this->Render();
      return true;
   };
   SInt32 EnumpickerUInt::IndexOf(const UInt32 value) const {
      auto s = this->values.size();
      for (decltype(s) i = 0; i < s; i++)
         if (this->values[i] == value)
            return i;
      return -1;
   };
   void EnumpickerUInt::Render() const {
      if (!this->tileValue)
         return;
      std::string value;
      this->Stringify(value, this->index);
      CALL_MEMBER_FN(this->tileValue, UpdateString)(kTileValue_string, value.c_str());
   };
   void EnumpickerUInt::Set(const UInt32 value, bool addIfMissing) {
      auto s = this->values.size();
      SInt32 i = this->IndexOf(value);
      if (i >= 0) {
         this->index = i;
         this->Render();
         return;
      }
      if (addIfMissing) {
         this->values.push_back(value);
         this->index = s;
         if (this->sorted) {
            this->Sort();
            i = this->IndexOf(value);
            this->index = i >= 0 ? i : 0;
         }
      } else {
         this->index = 0;
      }
      this->Render();
   };
   void EnumpickerUInt::Sort() {
      std::sort(this->values.begin(), this->values.end());
   };
   void EnumpickerUInt::Stringify(std::string& out, UInt32 index) const {
      std::stringstream ss;
      ss << this->values[index];
      out = ss.str();
   };
   //
   //
   //
   std::string EnumpickerStr::Get() const {
      if (!this->values.size())
         return "";
      return this->values[this->index];
   };
   bool EnumpickerStr::HandleClick(RE::Tile* tile) {
      auto s = this->values.size();
      if (tile == this->tileLeft) {
         this->index--; // if this goes below zero, it'll underflow
         if (this->index >= s)
            this->index = s - 1;
      } else if (tile == this->tileRight) {
         this->index++;
         if (this->index >= s)
            this->index = 0;
      } else
         return false;
      this->Render();
      return true;
   };
   SInt32 EnumpickerStr::IndexOf(const std::string& value) const {
      auto s = this->values.size();
      auto v = value.c_str();
      for (decltype(s) i = 0; i < s; i++) {
         auto& current = this->values[i];
         if (cobb::strieq(current.c_str(), v))
            return i;
      }
      return -1;
   };
   void EnumpickerStr::Render() const {
      if (!this->tileValue)
         return;
      const std::string& value = this->values[this->index];
      CALL_MEMBER_FN(this->tileValue, UpdateString)(kTileValue_string, value.c_str());
   };
   void EnumpickerStr::Set(const std::string& value, bool addIfMissing) {
      auto s = this->values.size();
      SInt32 i = this->IndexOf(value);
      if (i >= 0) {
         this->index = i;
         this->Render();
         return;
      }
      if (addIfMissing) {
         this->values.push_back(value);
         this->index = s;
         if (this->sorted) {
            this->Sort();
            i = this->IndexOf(value);
            this->index = i >= 0 ? i : 0;
         }
      } else {
         this->index = 0;
      }
      this->Render();
   };
   void EnumpickerStr::Sort() {
      std::sort(
         this->values.begin(),
         this->values.end(),
         [](std::string& a, std::string& b) { return _stricmp(a.c_str(), b.c_str()) < 0; }
      );
   };
   void EnumpickerStr::Stringify(std::string& out, UInt32 index) const {
      out = this->values[index];
   };
};