#include "Patches/XboxGamepad/CustomControls.h"
#include "Patches/XboxGamepad/Main.h"
#include "ReverseEngineered/Systems/Input.h"
#include "ReverseEngineered/INISettings.h"
#include "Miscellaneous/strings.h"
#include "Services/Translations.h"
#include "obse/Utilities.h"

#include <fstream>

const char* _controlNames[] = // must be in synch with Input.h's kControl enum
   {
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      "Use Equipped",
      "Activate",
      "Block",
      "Cast",
      "Draw/Sheathe",
      "Sneak",
      "Run",
      "Auto-Run",
      "Auto-Move",
      "Jump",
      "Toggle POV",
      "Big Four",
      "Wait",
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      nullptr, // don't let the player set this
      "Grab (Z-Key)",
   };

void XXNGamepadConfigManager::Profile::Set(std::string& key, std::string& value) {
   for (UInt32 i = 0; i < std::extent<decltype(_controlNames)>::value; i++) {
      auto s = _controlNames[i];
      if (!s)
         continue;
      if (_stricmp(s, key.c_str()) == 0) {
         auto code = XXNGamepad::GetButtonCode(value.c_str());
         this->bindings[i] = (UInt8)code;
         return;
      }
   }
};

const void _GetMyDocsPath(std::string& out) {
   char path[MAX_PATH];
   SHGetFolderPath(0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path);
   out = path;
   out += "\\My Games\\Oblivion\\";
};
const std::string& _GetPathCustom() {
   static std::string path;
   if (path.empty()) {
      _GetMyDocsPath(path);
      path += "NorthernUI.ctrl.txt";
   }
   return path;
};
const std::string& _GetPathBackup() {
   static std::string path;
   if (path.empty()) {
      _GetMyDocsPath(path);
      path += "NorthernUI.ctrl.bak";
   }
   return path;
};
const std::string& _GetPathWorking() {
   static std::string path;
   if (path.empty()) {
      _GetMyDocsPath(path);
      path += "NorthernUI.ctrl.tmp";
   }
   return path;
};
const std::string& _GetPathDefault() {
   static std::string path;
   if (path.empty()) {
      std::string	runtimePath = GetOblivionDirectory();
      if (!runtimePath.empty()) {
         path = runtimePath;
         path += "Data\\OBSE\\Plugins\\NorthernUI.ctrl.txt";
      }
   }
   return path;
};

void ParseLine(std::string& line, std::string& category, std::string& key, std::string& value) {
   UInt32 size = line.size();
   UInt32 i = 0;
   category = "";
   key      = "";
   value    = "";
   bool foundAny   = false;
   bool isCategory = false;
   do {
      char c = line[i];
      if (c == '\0' || c == '\r' || c == '\n')
         break;
      if (!foundAny) {
         if (isspace(c))
            continue;
         if (c == ';')
            break;
         if (c == '[') {
            isCategory = true;
            continue;
         }
         foundAny = true;
      }
      if (isCategory) {
         if (c == ']')
            return;
         category += c;
         continue;
      }
      if (c == '=') {
         key   = line.substr(0, i);
         value = line.substr(i + 1);
         {  // remove trailing "\r\n"
            char last = value.back();
            while (last == '\r' || last == '\n') {
               value.pop_back();
               last = value.back();
            }
         }
         return;
      }
   } while (++i < size);
   category = ""; // category wasn't finished with a ']'
};

void XXNGamepadConfigManager::ApplySelectedProfile(RE::OSInputGlobals* aInput) {
   if (!this->initialized)
      this->Init();
   Profile* profile = nullptr;
   {
      if (this->currentScheme.empty())
         profile = &(this->defaultProfile);
      else {
         profile = &(this->availableProfiles[this->currentScheme]);
         if (!profile)
            profile = &(this->defaultProfile);
      }
   }
   auto input = aInput; // assigning to the argument in the IF below didn't compile correctly -- I personally confirmed that in a disassembler. (it didn't actually ASSIGN input, though it did check if(input).)
   if (!input) {
      input = RE::OSInputGlobals::GetInstance();
      if (!input)
         _MESSAGE("XXNGamepadConfigManager::ApplySelectedProfile: Couldn't get OSInputGlobals! We will crash!");
   }
   input->joystick = *profile;
   {  // Apply pref: gameplay sticks
      RE::INI::Controls::iJoystickLookLeftRight->i = this->swapSticksGameplay ? 1 : 3;
      RE::INI::Controls::iJoystickLookUpDown->i    = this->swapSticksGameplay ? 2 : 6;
      RE::INI::Controls::iJoystickMoveLeftRight->i = this->swapSticksGameplay ? 3 : 1;
      RE::INI::Controls::iJoystickMoveFrontBack->i = this->swapSticksGameplay ? 6 : 2;
   }
};
void XXNGamepadConfigManager::CreateNewProfile(std::string& outProfileName) {
   outProfileName.clear();
   //
   std::string safe = NorthernUI::L10N::sControlsDefaultNewProfileName.value;
   try {
      this->availableProfiles.at(safe); // should throw if no profile by this name exists
      //
      // Try to find a profile name we can use, of the format "DefaultNewName (3)"
      //
      UInt32 i = 1;
      cobb::snprintf(safe, "%s (%d)", NorthernUI::L10N::sControlsDefaultNewProfileName.value, i);
      while (true) {
         this->availableProfiles.at(safe); // should throw if no profile by this name exists
         cobb::snprintf(safe, "%s (%d)", NorthernUI::L10N::sControlsDefaultNewProfileName.value, ++i);
         if (i > 30) // the user would have to work very, very hard to make this many profiles, but you never know
            break;   // anywho, if they did, then just fail
      }
   } catch (std::out_of_range) {
      this->availableProfiles[safe];
      outProfileName = safe;
   }
};
bool XXNGamepadConfigManager::CreateNewProfileByName(const std::string& profileName) {
   if (cobb::strieq(profileName, NorthernUI::L10N::sControlsDefaultProfileName.value))
      return false;
   try {
      this->availableProfiles.at(profileName); // should throw if no profile by this name exists
      return false;
   } catch (std::out_of_range) {
      this->availableProfiles[profileName];
      return true;
   }
};
bool XXNGamepadConfigManager::DeleteProfile(const std::string& target) {
   if (target.empty())
      return false;
   if (this->availableProfiles.erase(target)) {
      if (cobb::strieq(this->currentScheme, target))
         this->currentScheme = "";
      return true;
   }
   return false;
};
XXNGamepadConfigManager::Profile* XXNGamepadConfigManager::GetProfile(const std::string& name) {
   if (name.empty() || cobb::strieq(name, NorthernUI::L10N::sControlsDefaultProfileName.value))
      return nullptr;
   try {
      return &this->availableProfiles.at(name);
   } catch (std::out_of_range) {
      return nullptr;
   }
};
const XXNGamepadConfigManager::Profile* XXNGamepadConfigManager::GetProfileOrDefault(const std::string& name) const {
   if (name.empty() || cobb::strieq(name, NorthernUI::L10N::sControlsDefaultProfileName.value))
      return &this->defaultProfile;
   try {
      return &this->availableProfiles.at(name);
   } catch (std::out_of_range) {
      return nullptr;
   }
};
void XXNGamepadConfigManager::GetProfileNames(std::vector<std::string>& out) const {
   out.clear();
   out.reserve(1 + this->availableProfiles.size());
   out.push_back(NorthernUI::L10N::sControlsDefaultProfileName.value);
   for (auto it = this->availableProfiles.begin(); it != this->availableProfiles.end(); ++it)
      out.push_back(it->first);
};
void XXNGamepadConfigManager::Init() {
   std::lock_guard<std::mutex> lock(this->initLock); // lock scoped to function call
   if (this->initialized)
      return;
   this->initialized = true;
   this->currentScheme = "";
   this->LoadCustomProfiles();
   _MESSAGE("XXNGamepadConfigManager initialized.");
};
void XXNGamepadConfigManager::RenameScheme(const std::string& oldName, const std::string& newName) {
   if (oldName.empty() || newName.empty())
      return;
   try {
      this->availableProfiles.at(newName); // should throw if we don't have a profile by that name.
      return; // abort if the new name is taken
   } catch (std::out_of_range) {}
   try {
      Profile working = this->availableProfiles.at(oldName);
      this->availableProfiles[newName] = working;
   } catch (std::out_of_range) {
      return;
   }
   this->availableProfiles.erase(oldName);
   if (cobb::strieq(this->currentScheme, oldName))
      this->currentScheme = newName;
};
void XXNGamepadConfigManager::SetProfile(const std::string& name) {
   if (cobb::strieq(name, NorthernUI::L10N::sControlsDefaultProfileName.value)) {
      this->currentScheme = "";
      return;
   }
   this->currentScheme = name;
};

void XXNGamepadConfigManager::LoadCustomProfiles() {
   this->availableProfiles.clear();
   std::fstream file;
   file.open(_GetPathCustom(), std::ios_base::in);
   if (!file) {
      _MESSAGE("Unable to open gamepad config file for reading. Attempting to create the file...");
      this->SaveCustomProfiles();
      return;
   }
   std::string currentSchemeName;
   Profile     working;
   UInt32      version = 0;
   while (!file.bad() && !file.eof()) {
      std::string line;
      getline(file, line);
      line += '\n'; // add the terminator, since it (but not '\r') will be omitted from getline's result.
      //
      std::string key;
      std::string value;
      std::string category;
      ParseLine(line, category, key, value);
      if (!category.empty()) {
         if (!currentSchemeName.empty())
            this->availableProfiles[currentSchemeName] = working;
         std::swap(currentSchemeName, category); // fast assign
         working = Profile();
      } else if (!key.empty()) {
         cobb::rtrim(key);
         cobb::rtrim(value);
         //
         if (!currentSchemeName.empty()) {
            working.Set(key, value);
         } else {
            if (_stricmp(key.c_str(), "sUseSchemeName") == 0) {
               std::swap(this->currentScheme, value); // fast assign
               continue;
            }
            {
               bool isX = cobb::strieq(key.c_str(), "fSensitivityX");
               bool isY = cobb::strieq(key.c_str(), "fSensitivityY");
               if (isX || isY) {
                  float f;
                  if (!cobb::string_to_float(value.c_str(), f))
                     continue;
                  if (f) {
                     if (isX)
                        this->sensitivityX = f;
                     else
                        this->sensitivityY = f;
                  }
                  continue;
               }
            }
            SInt32 i;
            if (!cobb::string_to_int(value.c_str(), i, true))
               continue;
            const char* c = key.c_str();
            if (_stricmp(c, "bSwapSticksGameplay") == 0) {
               this->swapSticksGameplay = i;
            } else if (_stricmp(c, "bSwapSticksMenuMode") == 0) {
               this->swapSticksMenuMode = i;
            } else if (_stricmp(c, "iSensitivityRun") == 0) {
               this->sensitivityRun = i;
            } else if (_stricmp(c, "iVersion") == 0) {
               version = i;
            }
         }
      }
   }
   if (!currentSchemeName.empty())
      this->availableProfiles[currentSchemeName] = working;
   file.close();
   {  // Handle the possibility of the player somehow having named a user-defined profile after the default profile's display name.
      try {
         auto& profile = this->availableProfiles.at(NorthernUI::L10N::sControlsDefaultProfileName.value);
         UInt32 i = 1;
         std::string safe;
         cobb::snprintf(safe, "%s (%d)", NorthernUI::L10N::sControlsDefaultProfileName.value, i);
         try {
            //
            // Try to rename the profile from "Name" to "Name (3)" using the first available number 
            // starting from 1. However, don't go higher than 30; frankly that's already way too 
            // many times to be pinging the unordered_map.
            //
            while (true) {
               this->availableProfiles.at(safe); // should throw if no profile by this name exists
               cobb::snprintf(safe, "%s (%d)", NorthernUI::L10N::sControlsDefaultProfileName.value, ++i);
               if (i > 30) // the user would have to work very, very hard to make this many profiles, but you never know
                  break;   // anywho, if they did, then just discard this one bad profile
            }
         } catch (std::out_of_range) {
            //
            // We found a usable name of the format "Name (3)," so go ahead and rename the profile 
            // to that.
            //
            this->availableProfiles[safe] = profile;
         }
         //
         // Delete the offending profile.
         //
         this->availableProfiles.erase(NorthernUI::L10N::sControlsDefaultProfileName.value);
      } catch (std::out_of_range) {};
   }
   //
   // Handle version differences:
   //
   if (version < 0x01010000) { // versioning of the config file was introduced in v1.1.0
      //
      // Look sensitivity options were redesigned in 1.1.0 and are not backward-
      // compatible. The redesign prevented them from being frame-rate-sensitive, 
      // and had them specified as degrees per second in the INI file.
      //
      this->sensitivityX = 1.8F;
      this->sensitivityY = 1.0F;
   }
   if (version < 0x01020000) { // we didn't correctly generate missing files before v1.2.0; this primarily affects NorthernUIAway users
      this->SaveCustomProfiles();
   }
};
void XXNGamepadConfigManager::SaveCustomProfiles() {
   _MESSAGE("Writing to the user's gamepad config file...");
   std::string iPath = _GetPathCustom();
   std::string oPath = _GetPathWorking();
   std::fstream oFile;
   std::fstream iFile;
   oFile.open(oPath, std::ios_base::out | std::ios_base::trunc);
   if (!oFile) {
      _MESSAGE("Unable to open working gamepad config file for writing.");
      return;
   }
   {  // Write base settings.
      char line[64];
      snprintf(line, sizeof(line), "iVersion=0x%08X\n", g_pluginVersion);
      oFile.write(line, strlen(line));
      snprintf(line, sizeof(line), "%s=%d\n", "bSwapSticksGameplay", this->swapSticksGameplay);
      oFile.write(line, strlen(line));
      snprintf(line, sizeof(line), "%s=%d\n", "bSwapSticksMenuMode", this->swapSticksMenuMode);
      oFile.write(line, strlen(line));
      snprintf(line, sizeof(line), "%s=%f\n", "fSensitivityX", this->sensitivityX);
      oFile.write(line, strlen(line));
      snprintf(line, sizeof(line), "%s=%f\n", "fSensitivityY", this->sensitivityY);
      oFile.write(line, strlen(line));
      snprintf(line, sizeof(line), "%s=%u\n", "iSensitivityRun", this->sensitivityRun);
      oFile.write(line, strlen(line));
      {  // Write current profile name
         std::string line = "sUseSchemeName=";
         line += this->currentScheme.c_str();
         line += '\n';
         oFile.write(line.c_str(), line.size());
      }
   }
   oFile.put('\n');
   for (auto it = this->availableProfiles.begin(); it != this->availableProfiles.end(); ++it) {
      auto& profile = it->second;
      {  // Write profile name
         std::string line = "[";
         line += it->first;
         line += "]\n";
         oFile.write(line.c_str(), line.size());
      }
      //
      for (int i = 0; i < sizeof(Profile); i++) {
         if (!_controlNames[i])
            continue;
         const char* button;
         if (profile.bindings[i] == 0xFF)
            button = nullptr;
         else
            button = XXNGamepad::GetButtonName(profile.bindings[i]);
         if (!button)
            button = "NONE";
         char line[64];
         snprintf(line, sizeof(line), "%s=%s\n", _controlNames[i], button);
         oFile.write(line, strlen(line));
      }
      oFile.put('\n');
   }
   oFile.close();
   bool success = ReplaceFile(_GetPathCustom().c_str(), _GetPathWorking().c_str(), _GetPathBackup().c_str(), 0, 0, 0);
   if (!success) {
      success = MoveFileEx(_GetPathWorking().c_str(), _GetPathCustom().c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
   }
   if (!success) {
      _MESSAGE("Unable to commit gamepad config changes.");
   } else
      _MESSAGE("Gamepad config saved.");
};
void XXNGamepadConfigManager::GenerateDefaultsFile() {
   const char* sFileContents = \
      ";\n"\
      "; File for predefined control schemes besides the default.\n"\
      ";\n"\
      "[Bumper Jumper]\n"\
      "Use Equipped = RT\n"\
      "Block        = LT\n"\
      "Cast         = Y\n"\
      "Activate     = X\n"\
      "Sneak        = LS\n"\
      "Jump         = RB\n"\
      "Toggle POV   = A\n"\
      "Big Four     = RS\n"\
      "Wait         = Back\n"\
      "Draw/Sheathe Weapon = B\n"\
      "Run          = NONE\n"\
      "Auto-Run     = NONE\n"\
      "Auto-Move    = NONE\n"\
      "Grab (Z-Key) = LB\n";
   //
   std::fstream oFile;
   oFile.open(_GetPathDefault(), std::ios_base::out | std::ios_base::trunc);
   if (!oFile) {
      _MESSAGE("Unable to open gamepad config defaults file for writing.");
      return;
   }
   oFile.write(sFileContents, strlen(sFileContents));
   oFile.close();
   _MESSAGE("Gamepad config defaults generated from the hardcoded backup.");
};