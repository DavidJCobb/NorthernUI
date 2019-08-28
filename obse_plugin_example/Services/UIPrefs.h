#pragma once

namespace cobb {
   struct XMLDocument;
};

class UIPrefManager {
   public:
      static UIPrefManager& GetInstance() {
         static UIPrefManager instance;
         return instance;
      };

      enum PrefType {
         kPrefType_Float,
         kPrefType_String,
      };
      struct Pref {
         std::string name;
         float       defaultFloat = 0.0F;
         std::string defaultString;
         float       currentFloat = 0.0F;
         std::string currentString;
         PrefType type = kPrefType_Float;

         void initialize() {
            switch (this->type) {
               case kPrefType_Float:
                  this->currentFloat  = this->defaultFloat;
                  this->currentString = "";
                  break;
               case kPrefType_String:
                  this->currentString = this->defaultString;
                  this->currentFloat  = 0.0F;
                  break;
            }
         }
      };
      //
   private:
      std::vector<Pref> prefs;
      //
      Pref* getPrefByName(const char* name);
      const Pref* getPrefByName(const char* name) const;
      //
      void processDocument(cobb::XMLDocument&);
      //
   public:
      float getPrefFloatCurrentValue(const char* name) const;
      float getPrefFloatDefaultValue(const char* name) const;

      void dumpDefinitions() const;
      void loadDefinitions();
};