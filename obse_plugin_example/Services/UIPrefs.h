#pragma once
#include <string>
#include <unordered_map>

namespace cobb {
   struct XMLDocument;
};

#define XXN_UI_ALL_PREFS_ARE_FLOATS 1

class UIPrefManager {
   private:
      UIPrefManager(); // patches the engine for a menu-close hook
   public:
      static UIPrefManager& GetInstance() {
         static UIPrefManager instance;
         return instance;
      };

      struct Pref {
         float       defaultFloat = 0.0F;
         float       currentFloat = 0.0F;
         float       pendingFloat = 0.0F;
         UInt32      pendingChangesFromMenuID = 0; // what menu has written pending changes?

         void  commitPendingChanges(UInt32 closingMenuID);
         float getValue(UInt32 askingMenuID) const;

         Pref() {}
         Pref(float defaultFloat);
      };
      //
   private:
      std::unordered_map<std::string, Pref> prefs;
      //
      Pref* getPrefByName(std::string name);
      const Pref* getPrefByName(std::string name) const;
      //
      void processDocument(cobb::XMLDocument&);
      //
   public:
      float getPrefCurrentValue(const char* name, UInt32 askingMenuID = 0) const;
      float getPrefDefaultValue(const char* name) const;

      void resetPrefValue(const char* name, UInt32 menuID);
      void setPrefValue(const char* name, float v, UInt32 menuID);

      void dumpDefinitions() const;
      void loadDefinitions();

      void onMenuClose(UInt32 menuID);

      void loadUserValues();
      void saveUserValues() const;
};