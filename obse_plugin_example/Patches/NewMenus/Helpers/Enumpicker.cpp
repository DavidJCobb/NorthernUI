//#include "Enumpicker.h"

namespace XXNMenuHelpers {
   /*
   UInt32 EnumpickerUInt::Get() {
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
      std::string value;
      this->Stringify(value, this->index);
      CALL_MEMBER_FN(this->tileValue, UpdateString)(kTileValue_string, value.c_str());
   };
   void EnumpickerUInt::Set(const UInt32 value, bool addIfMissing) {
      auto s = this->values.size();
      if ((this->index = this->IndexOf(value)) >= 0) {
         this->Render();
         return;
      }
      if (addIfMissing) {
         this->values.push_back(value);
         this->index = s;
         if (this->sorted) {
            this->Sort();
            this->index = this->IndexOf(value);
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
   std::string EnumpickerStr::Get() {
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
         if (_stricmp(current.c_str(), v) == 0)
            return i;
      }
      return -1;
   };
   void EnumpickerStr::Render() const {
      const std::string& value = this->values[this->index];
      CALL_MEMBER_FN(this->tileValue, UpdateString)(kTileValue_string, value.c_str());
   };
   void EnumpickerStr::Set(const std::string& value, bool addIfMissing) {
      auto s = this->values.size();
      if ((this->index = this->IndexOf(value)) >= 0) {
         this->Render();
         return;
      }
      if (addIfMissing) {
         this->values.push_back(value);
         this->index = s;
         if (this->sorted) {
            this->Sort();
            this->index = this->IndexOf(value);
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
   */

   template<typename T> T Enumpicker<T>::Get() const {
      return this->values[this->index];
   };
   template<typename T> bool Enumpicker<T>::HandleClick(RE::Tile* tile) {
      auto s = this->values.size();
      if (tile == this->tileLeft) {
         this->index--; // if this goes below zero, it'll underflow
         ; // needed to prevent VS/MSVC syntax highlighter from choking
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
   template<typename T> SInt32 Enumpicker<T>::IndexOf(const T& value) const {
      auto s = this->values.size();
      for (decltype(s) i = 0; i < s; i++)
         if (this->values[i] == value)
            return i;
      return -1;
   };
   template<typename T> void Enumpicker<T>::Render() const {
      std::string value;
      this->Stringify(value, this->index);
      CALL_MEMBER_FN(this->tileValue, UpdateString)(kTileValue_string, value.c_str());
   };
   template<typename T> void Enumpicker<T>::Set(const T& value, bool addIfMissing) {
      auto s = this->values.size();
      if ((this->index = this->IndexOf(value)) >= 0) {
         this->Render();
         return;
      }
      if (addIfMissing) {
         this->values.push_back(value);
         this->index = s;
         if (this->sorted) {
            this->Sort();
            this->index = this->IndexOf(value);
         }
      } else {
         this->index = 0;
      }
      this->Render();
   };
   template<typename T> void Enumpicker<T>::Sort() {
      std::sort(this->values.begin(), this->values.end());
   };
   template<> void Enumpicker<std::string>::Sort() {
      std::sort(
         this->values.begin(),
         this->values.end(),
         [](std::string& a, std::string& b) { return _stricmp(a.c_str(), b.c_str()) < 0; }
      );
   };
   template<typename T> void Enumpicker<T>::Stringify(std::string& out, UInt32 index) const {
      static_assert(false, "This should never be called.");
   };
   template<> void Enumpicker<UInt32>::Stringify(std::string& out, UInt32 index) const {
      std::stringstream ss;
      ss << this->values[index];
      out = ss.str();
   };
   template<> void Enumpicker<std::string>::Stringify(std::string& out, UInt32 index) const {
      out = this->values[index];
   };
};