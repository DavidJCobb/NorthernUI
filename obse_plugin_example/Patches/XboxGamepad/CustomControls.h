#pragma once
#include "ReverseEngineered/Systems/Input.h"
#include "Patches/XboxGamepad/Main.h"

#include <initializer_list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class XXNGamepadConfigManager {
   private:
      XXNGamepadConfigManager() = default;
      XXNGamepadConfigManager(const XXNGamepadConfigManager&)            = delete; // no copying instances
      XXNGamepadConfigManager& operator=(const XXNGamepadConfigManager&) = delete; // no copying instances
      //
      void GenerateDefaultsFile();
      void LoadCustomProfiles();
      //
      std::mutex initLock;

   public:
      bool  initialized = false;
      std::string currentScheme; // outside callers should prefer SetProfile(...) over writing to this directly
      bool  swapSticksGameplay = false; // if true, then LS = look and RS = move
      bool  swapSticksMenuMode = false; // if true, then use RS for menus instead of LS
      float sensitivityX = 0.0005F; // recommended upper bound: 0.001
      float sensitivityY = 0.0003F;
      UInt8 sensitivityRun = 98; // magnitude > this = you're running
      //
      struct Profile : public RE::OSInputGlobals::Scheme {
         Profile() {
            memset(&this->bindings, 0xFF, sizeof(this->bindings));
         };
         Profile(std::initializer_list<UInt8> l) {
            assert(l.size() < RE::kControl_COUNT);
            UInt8 i = 0;
            for (auto it = l.begin(); it != l.end(); ++it) {
               this->bindings[i] = *it;
               i++;
            }
         };
         void Set(std::string& key, std::string& value);
      };
      //
      Profile defaultProfile = // Oblivion's default controls
         {
            0xFF, // Move Forward
            0xFF, // Move Backward
            0xFF, // Move Left
            0xFF, // Move Right
            XXNGamepad::kGamepadButton_SpecialCaseRT,
            XXNGamepad::kGamepadButton_A,
            XXNGamepad::kGamepadButton_SpecialCaseLT,
            XXNGamepad::kGamepadButton_RB,
            XXNGamepad::kGamepadButton_X,
            XXNGamepad::kGamepadButton_LS,
            0xFF, // Run
            0xFF, // Auto-Run
            0xFF, // Auto-Move
            XXNGamepad::kGamepadButton_Y,
            XXNGamepad::kGamepadButton_RS,
            XXNGamepad::kGamepadButton_B,
            XXNGamepad::kGamepadButton_Back,
            0xFF, // QuickKeysMenu
            0xFF, // Quick1
            0xFF, // Quick2
            0xFF, // Quick3
            0xFF, // Quick4
            0xFF, // Quick5
            0xFF, // Quick6
            0xFF, // Quick7
            0xFF, // Quick8
            0xFF, // Quicksave
            0xFF, // Quickload
            XXNGamepad::kGamepadButton_LB,
         };
      std::unordered_map<std::string, Profile> availableProfiles; // user-defined control schemes

      void ApplySelectedProfile(RE::OSInputGlobals* input = nullptr); // arg is needed because input is still being constructed when the game loads user control prefs, so we can't get it from the usual pointer
      void CreateNewProfile(std::string& outProfileName); // if it fails, outProfileName will be set to an empty string
      bool CreateNewProfileByName(const std::string& profileName); // returns false if name is taken
      bool DeleteProfile(const std::string& target); // returns success bool
      Profile* GetProfile(const std::string& name);
      const Profile* GetProfileOrDefault(const std::string& name) const;
      void GetProfileNames(std::vector<std::string>& out) const;
      void Init();
      void RenameScheme(const std::string& oldName, const std::string& newName);
      void SaveCustomProfiles();
      void SetProfile(const std::string& name);

      static XXNGamepadConfigManager& GetInstance() {
         static XXNGamepadConfigManager _core;
         return _core;
      };
};