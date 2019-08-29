#include "UIPrefs.h"
#include "Miscellaneous/debug.h"
#include "Miscellaneous/file.h"
#include "Miscellaneous/strings.h"
#include "Miscellaneous/xml.h"
#include "obse/Utilities.h" // GetOblivionDirectory
#include "obse_common/SafeWrite.h"
#include "ReverseEngineered\UI\Menu.h"

namespace {
   const std::string& GetPrefDirectory() {
      static std::string path;
      if (path.empty()) {
         std::string	runtimePath = GetOblivionDirectory();
         if (!runtimePath.empty()) {
            path = runtimePath;
            path += "Data\\NorthernUI\\prefs\\";
         }
      }
      return path;
   }
   const std::string& GetSavePath() {
      static std::string path;
      if (path.empty()) {
         std::string	runtimePath = GetOblivionDirectory();
         if (!runtimePath.empty()) {
            path = runtimePath;
            path += "Data\\NorthernUI\\prefs\\saved.ini";
         }
      }
      return path;
   }
}

namespace {
   void _stdcall HookInner(RE::Menu* menu) {
      if (menu)
         UIPrefManager::GetInstance().onMenuClose(menu->GetID());
   }
   __declspec(naked) void HookOuter() {
      _asm {
         mov  eax, 0x00584300; // reproduce patched-over call to FadeMenu
         call eax;
         push esi;
         call HookInner; // stdcall
         mov  eax, 0x0058479F;
         jmp  eax;
      }
   }
}
UIPrefManager::UIPrefManager() {
   WriteRelJump(0x0058479A, (UInt32)&HookOuter);
}

//

void UIPrefManager::Pref::commitPendingChanges(UInt32 closingMenuID) {
   if (this->pendingChangesFromMenuID == closingMenuID) {
      this->pendingChangesFromMenuID = 0;
      this->currentFloat = this->pendingFloat;
      this->pendingFloat = 0.0F;
   }
}
float UIPrefManager::Pref::getValue(UInt32 askingMenuID) const {
   /*//
   if (askingMenuID)
      if (this->pendingChangesFromMenuID == askingMenuID)
         return this->pendingFloat;
   //*/
   return this->currentFloat;
}
UIPrefManager::Pref::Pref(float defaultFloat) {
   this->defaultFloat = defaultFloat;
   this->currentFloat = defaultFloat;
}

//

UIPrefManager::Pref* UIPrefManager::getPrefByName(std::string name) {
   try {
      return &this->prefs.at(name);
   } catch (std::out_of_range) {
      return nullptr;
   }
}
const UIPrefManager::Pref* UIPrefManager::getPrefByName(std::string name) const {
   try {
      return &this->prefs.at(name);
   } catch (std::out_of_range) {
      return nullptr;
   }
}

float UIPrefManager::getPrefCurrentValue(const char* name, UInt32 askingMenuID) const {
   auto pref = this->getPrefByName(name);
   if (pref)
      return pref->getValue(askingMenuID);
   _MESSAGE("WARNING: Unable to load current value for non-existent pref \"%s\".", name ? name : "");
   return NAN;
}
float UIPrefManager::getPrefDefaultValue(const char* name) const {
   auto pref = this->getPrefByName(name);
   if (pref)
      return pref->defaultFloat;
   return NAN;
}
void UIPrefManager::resetPrefValue(const char* name, UInt32 menuID) {
   auto pref = this->getPrefByName(name);
   if (!pref)
      return;
   pref->pendingFloat = pref->defaultFloat;
   pref->pendingChangesFromMenuID = menuID;
}
void UIPrefManager::setPrefValue(const char* name, float v, UInt32 menuID) {
   auto pref = this->getPrefByName(name);
   if (!pref) {
      _MESSAGE("WARNING: Unable to save changes to non-existent pref \"%s\".", name ? name : "");
      return;
   }
   pref->pendingFloat = v;
   pref->pendingChangesFromMenuID = menuID;
}
//
void UIPrefManager::processDocument(cobb::XMLDocument& doc) {
   UInt32 nesting  = 0;
   bool   isInPref = false;
   //
   std::string prefName;
   float       prefDefault = 0.0F;
   //
   for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
      auto& token = *it;
      if (token.code == cobb::kXMLToken_ElementOpen) {
         if (nesting == 0) {
            if (token.name != "prefset") {
               _MESSAGE("ERROR: The root node must be a <prefset /> element. Aborting processing for this file.");
               //
               // TODO: decide on better error handling than just aborting.
               //
               return;
            }
         }
         if (token.name == "pref") {
            if (isInPref) {
               _MESSAGE("ERROR: <pref /> elements cannot be nested. Aborting processing for this file.");
               //
               // TODO: decide on better error handling than just aborting.
               //
               return;
            }
            isInPref = true;
         }
         ++nesting;
      } else if (token.code == cobb::kXMLToken_Attribute) {
         if (!isInPref)
            continue;
         if (token.name == "name") {
            std::swap(prefName, token.value); // we're never gonna use these tokens again, so just steal the strings with swap instead of wasting overhead on a copy-assign
            continue;
         } else if (token.name == "default") {
            auto  val = token.value.c_str();
            char* end = nullptr;
            float f = strtof(val, &end);
            if (end != val) {
               prefDefault = f;
               continue;
            }
            _MESSAGE("WARNING: Cannot assign value \"%s\" to pref \"%\". Values must be floats.", val, prefName.c_str()); // TODO: this message won't work if the default comes before the name
            continue;
         }
      } else if (token.code == cobb::kXMLToken_ElementClose) {
         --nesting;
         if (token.name != "pref")
            continue;
         if (prefName.size()) {
            if (this->getPrefByName(prefName.c_str())) {
               _MESSAGE("WARNING: Duplicate pref \"%s\" detected; discarding.", prefName.c_str());
            } else {
               this->prefs.emplace(prefName, prefDefault);
            }
            //this->prefs.emplace_back(std::string("TEST NAME TEST NAME"), 12345.0F);
            prefName = "";
         }
         prefDefault = 0.0F;
         isInPref = false;
      }
   }
}

void UIPrefManager::dumpDefinitions() const {
   _MESSAGE("Dumping list of prefs in UIPrefManager...");
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& name = it->first;
      auto& pref = it->second;
      _MESSAGE(" - %s=%f [default %f]", name.c_str(), pref.currentFloat, pref.defaultFloat);
   }
   _MESSAGE(" - Done.");
}
void UIPrefManager::loadDefinitions() {
   _MESSAGE("UIPrefManager::loadDefinitions()");
   const std::string& root = GetPrefDirectory();
   WIN32_FIND_DATA state;
   HANDLE handle;
   {
      std::string a = root + '*';
      handle = FindFirstFileA(a.c_str(), &state);
   }
   if (handle == INVALID_HANDLE_VALUE) {
      auto e = GetLastError();
      if (e != ERROR_FILE_NOT_FOUND) {
         _MESSAGE("FindFirstFile failed (%d)", e);
      }
      _MESSAGE(" - Done.");
      return;
   }
   cobb::XMLDocument doc;
   {  // Configure the XML document.
      //
      // TODO: add all UI XML entities; best if we don't do this manually but 
      // instead iterate through all entities that have been registered in-
      // memory; downside is we then need to delay this load process until 
      // after the UI engine has set itself up
      //
      doc.stripWhitespace = true;
   }
   do {
      if (state.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         //
         // We do not currently support subfolders.
         //
      } else {
         {  // Filename check: *.xml
            // Do the check ourselves: online searches say that FindFirstFile 
            // calls with "*.txt" are treated the same as "*.txt*", so we can't 
            // use that to do the filtering for us.
            size_t i = strlen(state.cFileName);
            if (i < 4)
               continue;
            if (_strnicmp(state.cFileName + i - 4, ".xml", 4) != 0)
               continue;
         }
         std::string path = root + state.cFileName;
         if (state.nFileSizeHigh) {
            _MESSAGE(" - File is too large: %s", path.c_str());
            continue;
         }
         std::ifstream file;
         file.open(path, std::ifstream::binary);
         if (!file) {
            _MESSAGE(" - Failed to open file: %s", path.c_str());
            continue;
         }
         std::string data;
         cobb::ifstream_to_string(data, file, state.nFileSizeLow);
         if (file.bad() || file.fail()) {
            file.close();
            data.clear();
            _MESSAGE(" - Failed to read file: %s", path.c_str());
            continue;
         }
         file.close();
         //
         cobb::parseXML(doc, data.c_str(), data.size());
         data.clear();
         this->processDocument(doc);
         doc.clear();
         //
         _MESSAGE(" - Loaded: %s", path.c_str());
      }
   } while (FindNextFileA(handle, &state));
   FindClose(handle);
   _MESSAGE(" - Done.");
}

void UIPrefManager::onMenuClose(UInt32 menuID) {
   bool needsSave = false;
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& pref = it->second;
      if (pref.pendingChangesFromMenuID == menuID)
         needsSave = true;
      pref.commitPendingChanges(menuID);
   }
   if (needsSave)
      this->saveUserValues();
}

namespace {
   constexpr char c_iniComment = ';';
   constexpr char c_iniCategoryStart = '[';
   constexpr char c_iniCategoryEnd   = ']';
   constexpr char c_iniKeyValueDelim = '=';
}
void UIPrefManager::loadUserValues() {
   _MESSAGE("Loading the user's UI prefs...");
   std::ifstream file;
   file.open(GetSavePath());
   if (!file) {
      _MESSAGE("Unable to open INI file for reading.");
      return;
   }
   std::string category;
   std::string key;
   while (!file.bad() && !file.eof()) {
      char buffer[1024];
      file.getline(buffer, sizeof(buffer));
      buffer[1023] = '\0';
      //
      bool   foundAny   = false; // found anything that isn't whitespace?
      bool   isCategory = false;
      bool   isValue    = false;
      UInt32 i = 0;
      char   c = buffer[0];
      if (!c)
         continue;
      do {
         if (!foundAny) {
            if (c == ' ' || c == '\t')
               continue;
            if (c == c_iniComment) // lines starting with semicolons are comments
               break;
            foundAny = true;
            if (c == c_iniCategoryStart) {
               isCategory = true;
               category   = "";
               continue;
            }
            key = "";
         }
         if (isCategory) {
            if (c == c_iniCategoryEnd)
               break;
            category += c;
            continue;
         }
         if (c == c_iniKeyValueDelim) {
            //
            // Found the first '=' in the setting; the next char is the start of the value.
            //
            i++;
            break;
         }
         key += c;
      } while (++i < sizeof(buffer) && (c = buffer[i]));
      if (!key.size())
         continue;
      auto pref = this->getPrefByName(key.c_str());
      if (!pref) {
         _MESSAGE("Discarding unrecognized pref \"%s\"...", key.c_str());
         continue;
      }
      //
      // Code from here on out assumes that (i) will not be modified -- it will always refer 
      // either to the end of the line or to the first character after the '='.
      //
      {  // Allow comments on the same line as a setting (but make sure we change this if we add string settings!)
         UInt32 j = i;
         do {
            if (buffer[j] == '\0')
               break;
            if (buffer[j] == ';') {
               buffer[j] = '\0';
               break;
            }
         } while (++j < sizeof(buffer));
      }
      float value;
      if (cobb::string_to_float(buffer + i, value)) {
         pref->currentFloat = value;
         pref->pendingFloat = 0.0F;
         pref->pendingChangesFromMenuID = 0;
      } else {
         _MESSAGE("Pref \"%s\" has an invalid value: \"%s\" is not a float.", key.c_str(), buffer + i);
      }
   }
   file.close();
   _MESSAGE("UI prefs loaded.");
}
void UIPrefManager::saveUserValues() const {
   _MESSAGE("Saving the user's UI prefs...");
   std::ofstream file;
   file.open(GetSavePath(), std::ios_base::out | std::ios_base::trunc);
   if (!file) {
      _MESSAGE("Unable to open INI file for writing.");
      return;
   }
   file << "[PrefValues]\n";
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& name = it->first;
      auto& pref = it->second;
      std::string line;
      cobb::snprintf(line, "%s=%f\n", name.c_str(), pref.currentFloat);
      file.write(line.c_str(), line.size());
   }
   file.close();
   _MESSAGE("UI prefs saved.");
}