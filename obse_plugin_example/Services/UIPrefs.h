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
         PrefType type = kPrefType_Float;
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
      float getPrefFloatDefaultValue(const char* name) const;
};

void _RunPrefXMLParseTest();