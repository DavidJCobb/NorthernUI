#include "UIPrefs.h"
#include "Miscellaneous/debug.h"
#include "Miscellaneous/file.h"
#include "Miscellaneous/strings.h"
#include "Miscellaneous/xml.h"
#include "obse/Utilities.h" // GetOblivionDirectory
#include "obse_common/SafeWrite.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "Patches/Selectors.h"

namespace {
   const std::string& GetPrefDirectory() { // folder for XML pref definitions
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
   const std::string& GetSavePath() { // save file for pref values altered at run-time
      static std::string path;
      if (path.empty()) {
         std::string	runtimePath = GetOblivionDirectory();
         if (!runtimePath.empty()) {
            path = runtimePath;
            path += "Data\\OBSE\\Plugins\\NorthernUI.xmlprefs.ini";
         }
      }
      return path;
   }

   bool _validatePrefName(const std::string& s) {
      if (!s.size())
         return false;
      if (s[0] == '_')
         return false;
      for (auto it = s.begin(); it != s.end(); ++it) {
         if (strchr(" /\\\"@()", *it)) // make sure error message in UIPrefManager::processDocument matches this character set
            return false;
      }
      return true;
   }
   std::string _prefNameToTraitName(const std::string& prefName) {
      std::string out = "_";
      out += prefName;
      return out;
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

bool UIPrefManager::Pref::commitPendingChanges(UInt32 closingMenuID) {
   if (this->pendingChangesFromMenuID == closingMenuID) {
      this->pendingChangesFromMenuID = 0;
      this->currentFloat = this->pendingFloat;
      this->pendingFloat = 0.0F;
      return true;
   }
   return false;
}
float UIPrefManager::Pref::getValue(UInt32 askingMenuID) const {
   if (this->pendingChangesFromMenuID)
      return this->pendingFloat;
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
   this->queuePrefPushToUIState(name);
}
void UIPrefManager::setPrefValue(const char* name, float v, UInt32 menuID) {
   auto pref = this->getPrefByName(name);
   if (!pref) {
      _MESSAGE("WARNING: Unable to save changes to non-existent pref \"%s\".", name ? name : "");
      return;
   }
   pref->pendingFloat = v;
   pref->pendingChangesFromMenuID = menuID;
   this->queuePrefPushToUIState(name);
}
void UIPrefManager::modifyPrefValue(const char* name, float v, UInt32 menuID) {
   auto pref = this->getPrefByName(name);
   if (!pref) {
      _MESSAGE("WARNING: Unable to save changes to non-existent pref \"%s\".", name ? name : "");
      return;
   }
   if (pref->pendingChangesFromMenuID) {
      pref->pendingFloat += v;
   } else
      pref->pendingFloat = pref->currentFloat + v;
   pref->pendingChangesFromMenuID = menuID;
   this->queuePrefPushToUIState(name);
}
void UIPrefManager::clampPrefValue(const char* name, float v, bool toMin, UInt32 menuID) {
   auto pref = this->getPrefByName(name);
   if (!pref) {
      _MESSAGE("WARNING: Unable to save changes to non-existent pref \"%s\".", name ? name : "");
      return;
   }
   float base = pref->currentFloat;
   if (pref->pendingChangesFromMenuID)
      base = pref->pendingFloat;
   if (toMin)
      base = (std::max)(v, base);
   else
      base = (std::min)(v, base);
   pref->pendingFloat = base;
   pref->pendingChangesFromMenuID = menuID;
   this->queuePrefPushToUIState(name);
}
//
void UIPrefManager::setupDocument(cobb::XMLDocument& doc) {
   doc.caseInsensitive = true;
   doc.stripWhitespace = true;
   //
   // Define all XML entities that exist in Oblivion XML.
   //
   auto& entities = RE::g_tagIDLists[27];
   for (auto node = entities.start; node; node = node->next) {
      auto entity = node->data;
      if (!entity)
         continue;
      if (entity->name.m_data[0] != '&') {
         _MESSAGE("[UIPrefManager::setupDocument] WARNING: Bad entity encountered.");
      }
      std::string value = std::to_string(entity->traitID);
      doc.defineEntity(entity->name.m_data, value.c_str());
   }
}
void UIPrefManager::processDocument(cobb::XMLDocument& doc) {
   UInt32 nesting  = 0;
   bool   isInPref = false;
   bool   showedNestWarning = false;
   //
   std::string prefName;
   float       prefDefault = 0.0F;
   bool        hasDefault = false;
   //
   for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
      auto& token = *it;
      if (token.code == cobb::kXMLToken_ElementOpen) {
         if (nesting == 0) {
            if (token.name != "prefset") {
               _MESSAGE("ERROR: Line %d: The root node must be a <prefset /> element. Aborting processing for this file.", token.line);
               return;
            }
         }
         if (token.name == "pref") {
            if (isInPref) {
               _MESSAGE("ERROR: Line %d: <pref /> elements cannot be nested. Aborting processing for this file.", token.line);
               return;
            }
            isInPref = true;
            //
            // TODO: ignore prefs that are not nested directly under the root node
            // (goal is so that if we extend the spec, we can put "extended" prefs 
            // under some child node without older NorthernUI versions logging tons 
            // of warnings; obviously we'll need to document that we're doing this)
            //
         }
         ++nesting;
      } else if (token.code == cobb::kXMLToken_Attribute) {
         if (!isInPref)
            continue;
         if (token.name == "name") {
            if (prefName.size())
               _MESSAGE("WARNING: Line %d: Loaded pref name \"%s\", but the current pref already has name \"%s\". The newly-loaded name is the name that will be retained.", token.line, token.value.c_str(), prefName.c_str());
            std::swap(prefName, token.value); // we're never gonna use these tokens again, so just steal the strings with swap instead of wasting overhead on a copy-assign
            continue;
         } else if (token.name == "default") {
            if (hasDefault) {
               if (prefName.size())
                  _MESSAGE("WARNING: Line %d: Pref \"%s\" has multiple default values; only the last one to load will be kept.", token.line, prefName.c_str());
               else
                  _MESSAGE("WARNING: Line %d: Pref, whose name has not yet loaded, has multiple default values; only the last one to load will be kept.", token.line);
            }
            hasDefault = true;
            //
            auto  val = token.value.c_str();
            char* end = nullptr;
            float f = strtof(val, &end);
            if (end != val) {
               prefDefault = f;
               continue;
            }
            if (prefName.size())
               _MESSAGE("WARNING: Line %d: Cannot assign value \"%s\" to pref \"%s\". Values must be floats.", token.line, val, prefName.c_str());
            else
               _MESSAGE("WARNING: Line %d: Cannot assign value \"%s\" to pref (whose name has not yet loaded). Values must be floats.", token.line, val);
            continue;
         }
      } else if (token.code == cobb::kXMLToken_ElementClose) {
         --nesting;
         if (token.name != "pref")
            continue;
         if (prefName.size()) {
            if (!_validatePrefName(prefName)) {
               _MESSAGE("WARNING: Line %d: Pref name \"%s\" contains disallowed characters; discarding. Pref names cannot begin with underscores, and cannot contain spaces, slashes, parentheses, at-symbols, or double-quotes.", token.line, prefName.c_str());
            } else if (this->getPrefByName(prefName.c_str())) {
               _MESSAGE("WARNING: Line %d: Duplicate pref \"%s\" detected; discarding.", token.line, prefName.c_str());
            } else {
               if (!hasDefault)
                  _MESSAGE("WARNING: Line %d: Loaded pref \"%s\" with no default value specified. Pref has been given the default value 0, but if that's what you want, then you really should specify it explicitly e.g. <pref name=\"%s\" default=\"0\" />.", token.line, prefName.c_str(), prefName.c_str());
               if (nesting > 1) {
                  //
                  // We are in a child of the root node.
                  //
                  // I want to ignore <pref /> elements that aren't direct children of the root 
                  // node as an extensibility measure: I want it to be possible to do something 
                  // like this in the future:
                  //
                  // <prefset>
                  //    <prefs version-min="3.0.0"> <!-- nothing in here is even seen by old NorthernUI -->
                  //       <pref name="foo" some-new-feature="bar" />
                  //    </prefs>
                  //    <pref name="baz" default="0" />
                  // </prefset>
                  //
                  // But I don't want to have to design that in advance; I want to handle it when 
                  // it comes up (if it comes up) so that I can handle it in the context of what-
                  // ever new feature I want to implement at that time.
                  //
                  if (!showedNestWarning) {
                     showedNestWarning = true;
                     _MESSAGE("WARNING: Line %d: Pref \"%s\" is not a direct child of the root node and will not be loaded. This warning will not be displayed again for this document.", token.line, prefName.c_str());
                  }
                  prefName    = "";
                  prefDefault = 0.0F;
                  hasDefault  = false;
                  isInPref    = false;
                  continue;
               }
               this->prefs.emplace(prefName, prefDefault);
            }
            prefName = "";
         } else {
            _MESSAGE("WARNING: Discarding pref lacking a name attribute. Element was closed on line %d.", token.line);
         }
         prefDefault = 0.0F;
         hasDefault = false;
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
   this->setupDocument(doc);
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
      auto& name = it->first;
      auto& pref = it->second;
      if (pref.commitPendingChanges(menuID)) {
         needsSave = true;
         this->pushPrefToUIState(name);
      }
   }
   if (needsSave)
      this->saveUserValues();
}

void UIPrefManager::queuePrefPushToUIState(const std::string& name) {
   auto& list = this->prefsPendingPushToUI;
   auto it = std::find(list.begin(), list.end(), name);
   if (it != list.end())
      return;
   list.push_back(name);
}
void UIPrefManager::pushPrefToUIState(const std::string& name) const {
   auto pref = this->getPrefByName(name);
   if (!pref)
      return;
   if (!g_northernUIPrefstore)
      return;
   std::string tName = _prefNameToTraitName(name);
   auto id = RE::GetOrCreateTempTagID(tName.c_str(), -1);
   if (pref->pendingChangesFromMenuID)
      CALL_MEMBER_FN(g_northernUIPrefstore, UpdateFloat)(id, pref->pendingFloat);
   else
      CALL_MEMBER_FN(g_northernUIPrefstore, UpdateFloat)(id, pref->currentFloat);
}
void UIPrefManager::pushQueuedPrefsToUIState() {
   if (g_northernUIPrefstore) {
      for (auto it = this->prefsPendingPushToUI.cbegin(); it != this->prefsPendingPushToUI.cend(); ++it) {
         const auto& name = *it;
         const auto  pref = this->getPrefByName(name);
         if (!pref)
            continue;
         std::string tName = _prefNameToTraitName(name);
         auto id = RE::GetOrCreateTempTagID(tName.c_str(), -1);
         if (pref->pendingChangesFromMenuID)
            CALL_MEMBER_FN(g_northernUIPrefstore, UpdateFloat)(id, pref->pendingFloat);
         else
            CALL_MEMBER_FN(g_northernUIPrefstore, UpdateFloat)(id, pref->currentFloat);
      }
   }
   this->prefsPendingPushToUI.clear();
}
void UIPrefManager::pushAllPrefsToUIState() const {
   if (!g_northernUIPrefstore)
      return;
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& name = it->first;
      auto& pref = it->second;
      std::string tName = _prefNameToTraitName(name);
      auto id = RE::GetOrCreateTempTagID(tName.c_str(), -1);
      if (pref.pendingChangesFromMenuID)
         CALL_MEMBER_FN(g_northernUIPrefstore, UpdateFloat)(id, pref.pendingFloat);
      else
         CALL_MEMBER_FN(g_northernUIPrefstore, UpdateFloat)(id, pref.currentFloat);
   }
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

void UIPrefManager::initialize() {
   auto& prefs = UIPrefManager::GetInstance();
   prefs.loadDefinitions();
   prefs.loadUserValues();
   prefs.dumpDefinitions(); // DEBUG DEBUG DEBUG
   prefs.pushAllPrefsToUIState();
}