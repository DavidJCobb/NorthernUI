#pragma once
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include "obse/Utilities.h"

namespace NorthernUI {
   namespace INI {
      //
      // NorthernUI.ini should be in the "Plugins" folder with the DLL.
      //
      // There's a callback system for changes; see bottom of header file
      //
      struct INISetting;
      //
      // INI SETTING DEFINITIONS.
      //
      // The macro used here intentionally differs from the one used in the CPP file. Keep the namespaces 
      // synchronized between the two files.
      //
      #define XXN_MAKE_INI_SETTING(name, category, value) extern INISetting name;
      namespace Core {
         XXN_MAKE_INI_SETTING(bPatchTextureFiltering, "Core", true);
         XXN_MAKE_INI_SETTING(uTextureFilteringFlags, "Core", 256);
      };
      namespace Debug {
         XXN_MAKE_INI_SETTING(bSwipeUnusedVanillaLogs, "Debug", false);
      };
      namespace Display {
         XXN_MAKE_INI_SETTING(bForceNativeResolutionUI,   "Display", false);
         XXN_MAKE_INI_SETTING(uLocalMapResolutionPerCell, "Display", 1024);
      };
      namespace Features {
         XXN_MAKE_INI_SETTING(bShowHUDClock,       "Features", true);
         XXN_MAKE_INI_SETTING(bSuppressDLCStartup, "Features", false);
      };
      namespace Menus {
         XXN_MAKE_INI_SETTING(bUseXXNAlchemyMenu,         "Menus", true);
         XXN_MAKE_INI_SETTING(bUseXXNSampleInventoryMenu, "Menus", false);
      };
      namespace XInput {
         XXN_MAKE_INI_SETTING(bEnabled,                 "XInput", true);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateInitial, "XInput", 300);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateMod,     "XInput", 75);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateMin,     "XInput", 75);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateUsedFor, "XInput", 3);
         XXN_MAKE_INI_SETTING(fMapMenuPanSpeed,         "XInput", 4.5F);
      };
      #undef XXN_MAKE_INI_SETTING
      //
      // INTERNALS BELOW.
      //
      class INISettingManager {
         private:
            typedef std::vector<INISetting*> VecSettings;
            typedef std::unordered_map<std::string, VecSettings> MapSettings;
            VecSettings settings;
            MapSettings byCategory;
            //
            struct _CategoryToBeWritten { // state object used when saving INI settings
               _CategoryToBeWritten() {};
               _CategoryToBeWritten(std::string name, std::string header) : name(name), header(header) {};
               //
               std::string name;   // name of the category, used to look up INISetting pointers for found setting names
               std::string header; // the header line, including whitespace and comments
               std::string body;   // the body
               VecSettings found;  // found settings
               //
               void Write(INISettingManager* const, std::fstream&);
            };
            //
            VecSettings& GetCategoryContents(std::string category);
            //
         public:
            static INISettingManager& GetInstance();
            //
            void Add(INISetting* setting);
            void Load();
            void Save(); // UNFINISHED -- SEE BOTTOM. IT WRITES TO A TEMPORARY FILE; WE NEED TO SWAP THE FILES AT THE END OF THE PROCESS. THEN WE NEED TO REMOVE THE CALL TO THIS FUNCTION FROM THE END OF Load.
            //
            INISetting* Get(std::string& category, std::string& name) const;
            INISetting* Get(const char*  category, const char*  name) const;
            void ListCategories(std::vector<std::string>& out) const;
      };
      struct INISetting {
         INISetting(const char* name, const char* category, bool   v) : name(name), category(category), bDefault(v), bCurrent(v), type(kType_Bool) {
            INISettingManager::GetInstance().Add(this);
         };
         INISetting(const char* name, const char* category, float  v) : name(name), category(category), fDefault(v), fCurrent(v), type(kType_Float) {
            INISettingManager::GetInstance().Add(this);
         };
         INISetting(const char* name, const char* category, int    v) : name(name), category(category), iDefault(v), iCurrent(v), type(kType_SInt) {
            INISettingManager::GetInstance().Add(this);
         };
         INISetting(const char* name, const char* category, SInt32 v) : name(name), category(category), iDefault(v), iCurrent(v), type(kType_SInt) {
            INISettingManager::GetInstance().Add(this);
         };

         enum ValueType {
            kType_Bool  = 0,
            kType_Float = 1,
            kType_SInt  = 2,
         };
         union {
            bool   bCurrent;
            float  fCurrent;
            SInt32 iCurrent;
         };
         const char* const name;
         const char* const category;
         union {
            bool   bDefault;
            float  fDefault;
            SInt32 iDefault;
         };
         const ValueType type;
         //
         std::string ToString() const;
      };
      //
      // Change callback system.
      // If (setting) is nullptr, then it's a generic change event. (For now, specific change events are not 
      // used or supported, but I want room to add them later.)
      //
      union ChangeCallbackArg {
         bool   b;
         float  f;
         SInt32 i;
         //
         ChangeCallbackArg(bool b) : b(b) {};
         ChangeCallbackArg(float f) : f(f) {};
         ChangeCallbackArg(int i) : i(i) {};
      };
      typedef void(*ChangeCallback)(INISetting* setting, ChangeCallbackArg oldValue, ChangeCallbackArg newValue);
      extern  void RegisterForChanges  (ChangeCallback c);
      extern  void UnregisterForChanges(ChangeCallback c);
      extern  void SendChangeEvent();
   };
};