#pragma once
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include "obse/Utilities.h"

namespace NorthernUI {
   enum BarterConfirmHandler : SInt32 {
      kBarterConfirmHandler_Always    = 0, // vanilla: always confirm a transaction
      kBarterConfirmHandler_IfNotFree = 1, // only show a confirmation prompt if the items have value
      kBarterConfirmHandler_Never     = 2, // never show a confirmation prompt
   };
   enum QuantityMenuHandler : SInt32 {
      kQuantityHandler_Default = 0, // show prompt
      kQuantityHandler_TakeOne = 1,
      kQuantityHandler_TakeAll = 2,
   };
   enum EnhancedMovementCameraType : SInt32 {
      kEnhancedMovementCameraType_OblivionStandard = 0,
      kEnhancedMovementCameraType_SkyrimStandard   = 1,
      kEnhancedMovementCameraType_FreeMovement     = 2,
   };
   enum CameraInertiaMode : SInt32 {
      kCameraInertiaMode_Standard = 0,
      kCameraInertiaMode_Fixed    = 1,
      kCameraInertiaMode_Disabled = 2,
   };

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
         XXN_MAKE_INI_SETTING(fAutoHideCursorDelay,       "Display", 5.0F);
         XXN_MAKE_INI_SETTING(uLocalMapResolutionPerCell, "Display", 1024);
      };
      namespace Features {
         XXN_MAKE_INI_SETTING(iQuantityMenuHandlerAlt,      "Features", kQuantityHandler_Default);
         XXN_MAKE_INI_SETTING(iQuantityMenuHandlerCtrl,     "Features", kQuantityHandler_Default);
         XXN_MAKE_INI_SETTING(iQuantityMenuHandlerDefault,  "Features", kQuantityHandler_Default);
         XXN_MAKE_INI_SETTING(iBarterConfirmHandlerAlt,     "Features", kBarterConfirmHandler_Always);
         XXN_MAKE_INI_SETTING(iBarterConfirmHandlerCtrl,    "Features", kBarterConfirmHandler_Always);
         XXN_MAKE_INI_SETTING(iBarterConfirmHandlerDefault, "Features", kBarterConfirmHandler_Always);
         XXN_MAKE_INI_SETTING(bShowHUDClock,       "Features", true);
         XXN_MAKE_INI_SETTING(bShowHUDInputViewer, "Features", false);
         XXN_MAKE_INI_SETTING(bSuppressDLCStartup, "Features", false);
         XXN_MAKE_INI_SETTING(bUsePlaystationButtonIcons, "Features", false);
         XXN_MAKE_INI_SETTING(bEnhancedMovement360Movement, "Features", false);
         XXN_MAKE_INI_SETTING(iEnhancedMovementCameraMode, "Features", kEnhancedMovementCameraType_OblivionStandard);
         XXN_MAKE_INI_SETTING(iCameraInertiaMode, "Features", kCameraInertiaMode_Fixed);
      };
      namespace Menus {
         XXN_MAKE_INI_SETTING(bUseXXNAlchemyMenu,         "Menus", true);
         XXN_MAKE_INI_SETTING(bUseXXNSampleInventoryMenu, "Menus", false);
      };
      namespace XInput {
         XXN_MAKE_INI_SETTING(bEnabled,                 "XInput", true);
         XXN_MAKE_INI_SETTING(bDontUseEvenWhenPatched,  "XInput", false);
         XXN_MAKE_INI_SETTING(bToggleAlwaysRunWorks,    "XInput", false); // controls whether Toggle Always Run affects joystick-initiated movement
         XXN_MAKE_INI_SETTING(bMenuConsumesDPad,        "XInput", true);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateInitial, "XInput", 300);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateMod,     "XInput", 75);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateMin,     "XInput", 75);
         XXN_MAKE_INI_SETTING(uMenuJoystickRateUsedFor, "XInput", 3);
         XXN_MAKE_INI_SETTING(fMapMenuPanSpeed,         "XInput", 4.5F);
         XXN_MAKE_INI_SETTING(fJoystickSensAccelOffset, "XInput", 0.8F);
         XXN_MAKE_INI_SETTING(fJoystickSensAccelTime,   "XInput", 1.0F);
         XXN_MAKE_INI_SETTING(fLookSensitivityMin,       "XInput", 1.0F);
         XXN_MAKE_INI_SETTING(fLookSensitivityStep,      "XInput", 0.8F);
         XXN_MAKE_INI_SETTING(iLookSensitivityStepCount, "XInput", 6);
         XXN_MAKE_INI_SETTING(iPlayerMenuModelJoystickMaxRotateSpeed, "XInput", 400); // mouse pixels per second
         XXN_MAKE_INI_SETTING(iPlayerMenuModelJoystickMaxZoomSpeed,   "XInput", 200); // mouse pixels per second
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
            void Save();
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