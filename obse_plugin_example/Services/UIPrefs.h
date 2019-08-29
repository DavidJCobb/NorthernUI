#pragma once
#include <string>
#include <unordered_map>
#include <vector>

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

         bool  commitPendingChanges(UInt32 closingMenuID); // returns true if there were changes to commit
         float getValue(UInt32 askingMenuID) const;

         Pref() {}
         Pref(float defaultFloat);
      };
      //
   private:
      std::unordered_map<std::string, Pref> prefs;
      std::vector<std::string> prefsPendingPushToUI;
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
      void modifyPrefValue(const char* name, float v, UInt32 menuID);
      void clampPrefValue(const char* name, float v, bool toMin, UInt32 menuID);

      void dumpDefinitions() const;
      void loadDefinitions();

      void onMenuClose(UInt32 menuID);

      void loadUserValues();
      void saveUserValues() const;

      void queuePrefPushToUIState(const std::string& name);
      void pushPrefToUIState(const std::string& name) const;
      void pushQueuedPrefsToUIState();
      void pushAllPrefsToUIState() const;
};