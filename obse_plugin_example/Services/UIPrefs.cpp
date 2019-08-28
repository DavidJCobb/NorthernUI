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
   do {
      if (state.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         //
         // We do not currently support subfolders.
         //
      } else {
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
         cobb::XMLDocument doc;
         //
         // TODO: add all UI XML entities
         //
         doc.stripWhitespace = true;
         //
         cobb::parseXML(doc, data.c_str(), data.size());
         this->processDocument(doc);
         data.clear();
         _MESSAGE(" - Loaded: %s", path.c_str());
      }
   } while (FindNextFileA(handle, &state));
   FindClose(handle);
   _MESSAGE(" - Done.");
}








void _RunPrefXMLParseTest() {
   const char* code[] = {
      "<root>\n   Foo\n</root>",
      "<root attr='value'>\n   Foo\n</root>",
      "<root attr='value'>\n   <foo />\n   <bar></bar>\n</root>",
      "<root>&amp;</root>",
      "<root>&#45;</root>",
      "<root>&lt;fake-nested&gt;</root>",
      "<root attr='&lt;value&gt;'></root>",
      "<!-- comment test -->",
      "<!-- <root> -->",
      "<!-- & -->",
      "<!---->",
      "<!--<![CDATA[ test ]]>-->",
      "<root><![CDATA[<nested>]]></root>",
      "<<![CDATA[root]]> attr='value'>foo</root>",
      "<root <![CDATA[attr]]>='value'>foo</root>",
      "<root attr='<![CDATA[value]]>'>foo</root>",
      "<root attr='value'>foo</<![CDATA[root]]>>",
      "<<![CDATA[ro!ot]]> attr='value'>foo</<![CDATA[ro!ot]]>>",
      "<ui> <trait>&true;</trait> <trait> &false; </trait> </ui>",
      "<?xml version=\"1.0\" ?><root></root>",
      "<root attr='value\">\n   <foo />\n   <bar></bar>\n</root>", // mismatched quotes on the attribute value
      "<root", // unterminated opening tag
      "<", // lone angle bracket
      "<root></root", // unterminated closing tag
      "<root>\n   <nested>\n</root>", // unterminated inner element
      "<root>\n<nested/>", // mismatched closing tags
      "<root>&</root>", // lone ampersand
      "<root>&amp</root>", // unterminated entity
      "<root>&#;</root>", // character entity with no character code
      "<root>&#x;</root>", // character entity with no character code (hexadecimal)
      "<root>&#x00;</root>", // character entity using a null character code
      "<root>&fake;</root>", // unrecognized entity
      "<!-- comment test", // unterminated comment
      "<!-->", // incorrect "compact" comment
      "<!-- -- -->", // "--" is not allowed in comments
      " <?xml version=\"1.0\" ?><root></root>", // XML declaration is not at the very start of the file
   };
   _MESSAGE("== RUNNING PREF PARSE TESTS ==");
   for (UInt32 i = 0; i < std::extent<decltype(code)>::value; i++) {
      _MESSAGE("===== XML PARSE TEST %2d =====", i);
      //
      cobb::XMLDocument doc;
      doc.stripWhitespace = true;
      doc.defineEntity("&true;",  "2");
      doc.defineEntity("&false;", "1");
      //
      if (cobb::parseXML(doc, code[i], strlen(code[i]))) {
         _MESSAGE("DOCUMENT PARSING COMPLETE. REPLICATING...");
         bool unclosed = false;
         for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
            auto& token = *it;
            switch (token.code) {
               case cobb::kXMLToken_TextContent:
                  if (unclosed) {
                     _MESSAGE(">");
                     unclosed = false;
                  }
                  _MESSAGE(token.value.c_str());
                  break;
               case cobb::kXMLToken_ElementOpen:
                  unclosed = true;
                  _MESSAGE("<%s", token.name.c_str());
                  break;
               case cobb::kXMLToken_Attribute:
                  _MESSAGE(" %s=\"%s\"", token.name.c_str(), token.value.c_str());
                  break;
               case cobb::kXMLToken_ElementClose:
                  if (unclosed) {
                     _MESSAGE(">");
                     unclosed = false;
                  }
                  _MESSAGE("</%s>", token.name.c_str());
                  break;
            }
         }
         _MESSAGE("END OF DOCUMENT");
      }
      _MESSAGE("\n");
   }
   _MESSAGE("== ALL PARSE TESTS COMPLETE ==");
}