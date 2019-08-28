#include "UIPrefs.h"
#include "Miscellaneous/file.h"
#include "Miscellaneous/xml.h"
#include "obse/Utilities.h" // GetOblivionDirectory

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

UIPrefManager::Pref* UIPrefManager::getPrefByName(const char* name) {
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& pref = *it;
      if (pref.name == name)
         return &pref;
   }
   return nullptr;
}
const UIPrefManager::Pref* UIPrefManager::getPrefByName(const char* name) const {
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& pref = *it;
      if (pref.name == name)
         return &pref;
   }
   return nullptr;
}
float UIPrefManager::getPrefFloatCurrentValue(const char* name) const {
   auto pref = this->getPrefByName(name);
   if (pref)
      return pref->currentFloat;
   return NAN;
}
float UIPrefManager::getPrefFloatDefaultValue(const char* name) const {
   auto pref = this->getPrefByName(name);
   if (pref)
      return pref->defaultFloat;
   return NAN;
}
//
void UIPrefManager::processDocument(cobb::XMLDocument& doc) {
   Pref   currentPref;
   UInt32 nesting  = 0;
   bool   isInPref = false;
   for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
      auto& token = *it;
      if (token.code == cobb::kXMLToken_ElementOpen) {
         if (nesting == 0) {
            if (token.name != "prefset") {
               _MESSAGE("ERROR: The root node must be a <prefset /> element.");
               //
               // TODO: decide on better error handling than just aborting.
               //
               return;
            }
         }
         if (token.name == "pref") {
            if (isInPref) {
               _MESSAGE("ERROR: <pref /> elements cannot be nested.");
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
            std::swap(currentPref.name, token.value); // we're never gonna use these tokens again, so just steal the strings with swap instead of wasting overhead on a copy-assign
            continue;
         } else if (token.name == "default") {
            auto  val = token.value.c_str();
            char* end = nullptr;
            float f = strtof(val, &end);
            if (end != val) {
               currentPref.defaultFloat = f;
               continue;
            }
            std::swap(currentPref.defaultString, token.value); // we're never gonna use these tokens again, so just steal the strings with swap instead of wasting overhead on a copy-assign
            currentPref.type = kPrefType_String;
            continue;
         }
      } else if (token.code == cobb::kXMLToken_ElementClose) {
         --nesting;
         if (token.name != "pref")
            continue;
         if (currentPref.name.size()) {
            currentPref.initialize();
            this->prefs.push_back(currentPref);
         }
         currentPref = Pref();
         isInPref = false;
      }
   }
}

void UIPrefManager::dumpDefinitions() const {
   _MESSAGE("Dumping list of prefs in UIPrefManager...");
   for (auto it = this->prefs.begin(); it != this->prefs.end(); ++it) {
      auto& pref = *it;
      if (pref.type == kPrefType_Float) {
         _MESSAGE(" - %s=%f [default %f]", pref.name.c_str(), pref.currentFloat, pref.defaultFloat);
      } else if (pref.type == kPrefType_String) {
         _MESSAGE(" - %s=%s", pref.name.c_str(), pref.currentString.c_str());
      }
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
      // TODO: add all UI XML entities
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
         this->processDocument(doc);
         doc.clear();
         //
         data.clear();
         _MESSAGE(" - Loaded: %s", path.c_str());
      }
   } while (FindNextFileA(handle, &state));
   FindClose(handle);
   _MESSAGE(" - Done.");
}