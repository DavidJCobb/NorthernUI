#include "Translations.h"
#include "Miscellaneous/strings.h"

#include <fstream>
using namespace std;

#include "obse/Utilities.h"

namespace NorthernUI {
   namespace L10N {
      Str const sNotImplemented = Str("sNotImplemented", "NOT IMPLEMENTED");
      Str const sOrdinalSuffixLower0 = Str("sOrdinalSuffixLower0", "th");
      Str const sOrdinalSuffixLower1 = Str("sOrdinalSuffixLower1", "st");
      Str const sOrdinalSuffixLower2 = Str("sOrdinalSuffixLower2", "nd");
      Str const sOrdinalSuffixLower3 = Str("sOrdinalSuffixLower3", "rd");
      Str const sOrdinalSuffixLower4 = Str("sOrdinalSuffixLower4", "th");
      Str const sOrdinalSuffixUpper0 = Str("sOrdinalSuffixUpper0", "TH");
      Str const sOrdinalSuffixUpper1 = Str("sOrdinalSuffixUpper1", "ST");
      Str const sOrdinalSuffixUpper2 = Str("sOrdinalSuffixUpper2", "ND");
      Str const sOrdinalSuffixUpper3 = Str("sOrdinalSuffixUpper3", "RD");
      Str const sOrdinalSuffixUpper4 = Str("sOrdinalSuffixUpper4", "TH");
      Str const sTimeSuffixLowerAM = Str("sTimeSuffixLowerAM", "am");
      Str const sTimeSuffixLowerPM = Str("sTimeSuffixLowerPM", "pm");
      Str const sTimeSuffixUpperAM = Str("sTimeSuffixUpperAM", "AM");
      Str const sTimeSuffixUpperPM = Str("sTimeSuffixUpperPM", "PM");
      Str const sControlsKeyConflict = Str("sControlsKeyConflict", "That button is already assigned to %s. Swap these keys?", "s");
      Str const sControlsKeyConflictConfirm = Str("sControlsKeyConflictConfirm", "Swap Keys");
      Str const sControlsSwitchSchemeUnsaved = Str("sControlsSwitchSchemeUnsaved", "You haven't saved your changes to the current control scheme. Are you sure you want to switch to a different scheme? Your unsaved changes will be lost.");
      Str const sControlsDefaultProfileName = Str("sControlsDefaultProfileName", "Oblivion Default");
      Str const sControlsDefaultNewProfileName = Str("sControlsDefaultNewProfileName", "Untitled");
      Str const sControlsExitUnsaved = Str("sControlsExitUnsaved", "You haven't saved your changes to the %s control scheme. What would you like to do?", "s");
      Str const sControlsExitUnsavedCancel = Str("sControlsExitUnsavedCancel", "Cancel");
      Str const sControlsExitUnsavedNoSave = Str("sControlsExitUnsavedNoSave", "Exit Without Saving");
      Str const sControlsExitUnsavedSave = Str("sControlsExitUnsavedSave", "Save and Exit");
      Str const sControlsExitUnsavedSaveAsNew = Str("sControlsExitUnsavedSaveAsNew", "Save as New, and Exit");
      Str const sControlsDelete = Str("sControlsDelete", "Are you sure you want to delete the %s control scheme?", "s");
      Str const sControlsExitSaveAsNewNameTaken = Str("sControlsExitSaveAsNewNameTaken", "A profile named %s already exists.\n\nThere are unsaved changes to the %s control scheme. What would you like to do?", "ss");
      Str const sControlsCreateNamePrompt = Str("sControlsCreateNamePrompt", "What would you like to name the new control scheme?");
      Str const sControlsRenamePrompt = Str("sControlsRenamePrompt", "What would you like to rename the %s control scheme to?", "s");
      Str const sControlsNameCannotBeBlank = Str("sControlsNameCannotBeBlank", "A control scheme's name cannot be blank.");
      Str const sControlsNameIsTaken = Str("sControlsNameIsTaken", "There is already a control scheme named %s.", "s");
      Str const sControlsSaveAsNewButItsTheSameName = Str("sControlsSaveAsNewButItsTheSameName", "Save changes to the %s control scheme?", "s");
      Str const sControlsCreateNameTaken = Str("sControlsCreateNameTaken", "A control scheme named %s already exists. Enter a different name.", "s");
      Str const sControlsCreateUnsaved = Str("sControlsCreateUnsaved", "You haven't saved your changes to the %s control scheme. What would you like to do before creating a new control scheme?", "s");
      Str const sControlsCreateSaveFirst = Str("sControlsCreateSaveFirst", "Save Changes");
      Str const sControlsCreateDiscard = Str("sControlsCreateDiscard", "Discard Changes");
      Str const sResetOptionsToDefault = Str("sResetOptionsToDefault", "Reset all options to their default values?");
      Str const sBadMenuQueVersion = Str("sBadMenuQueVersion", "You are using an unrecognized version of MenuQue, so some NorthernUI features will not work. If you aren't using the newest version, please update. If you're using the newest version, notify NorthernUI's author so that they can update their mod.");
      //
      //
      // The machinery to make all that work is below:
      //
      //
      Str::Str(const char* name, const char* value) : label(name) {
         this->value = _strdup(value);
         StrManager::GetInstance().Add(name, this);
      };
      Str::Str(const char* name, const char* value, const char* tokens) : label(name), tokens(tokens) {
         this->value = _strdup(value);
         StrManager::GetInstance().Add(name, this);
      };
      void Str::Set(const char* value) {
         if (this->value)
            delete this->value;
         this->value = _strdup(value);
      };
      void Str::Set(std::string& value) {
         if (this->value)
            delete this->value;
         this->value = _strdup(value.c_str());
      };
      //
      StrManager& StrManager::GetInstance() {
         static StrManager instance;
         return instance;
      };
      void StrManager::Add(const char* name, Str* value) {
         this->strings[name] = value;
      };
      void StrManager::GetSortedKeys(std::vector<std::string>& out) const {
         out.clear();
         out.reserve(this->strings.size());
         for (auto it = this->strings.begin(); it != this->strings.end(); ++it)
            out.push_back(it->first);
         out.shrink_to_fit();
         std::sort(out.begin(), out.end(), [](std::string& a, std::string& b) { return _stricmp(a.c_str(), b.c_str()) < 0; });
      };
      void StrManager::Update() {
         std::string path;
         {
            std::string	runtimePath = GetOblivionDirectory();
            if (runtimePath.empty())
               return;
            path = runtimePath;
            path += "Data\\OBSE\\Plugins\\NorthernUI.l10n.txt";
         }
         std::ifstream file;
         file.open(path);
         if (!file) {
            _MESSAGE("Unable to load file: Data/OBSE/Plugins/NorthernUI.l10n.txt. We will generate a default one.");
            WriteFile();
            return;
         }
         while (!file.bad() && !file.eof()) {
            char buffer[512];
            file.getline(buffer, sizeof(buffer));
            //
            {
               UInt32 i = 0;
               char   c = buffer[0];
               if (!c)
                  continue;
               bool foundAny = false;
               bool isKey = true;
               std::string key;
               std::string value;
               do {
                  if (!foundAny) {
                     if (c == ' ' || c == '\t')
                        continue;
                     if (c == ';') // lines starting with semicolons are comments
                        break;
                     foundAny = true;
                  }
                  if (foundAny) {
                     if (isKey) {
                        if (c == '=') {
                           isKey = false;
                           continue;
                        }
                        key += c;
                     } else {
                        if (c == '\\' && i + 1 < sizeof(buffer)) {
                           //
                           // Handle supported backslash-escape codes.
                           //
                           if (buffer[i + 1] == 'n') {
                              value += '\n';
                              i++;
                              continue;
                           }
                        }
                        value += c;
                     }
                  }
               } while (++i < sizeof(buffer) && (c = buffer[i]));
               try {
                  auto stored = this->strings.at(key);
                  if (stored->tokens) {
                     if (!cobb::validate_format(value, stored->tokens)) {
                        _MESSAGE("Format string parameters in %s are wrong. Expected params in the order: %s.", key.c_str(), stored->tokens);
                        continue;
                     }
                  }
                  stored->Set(value);
               } catch (std::out_of_range) {}
            }
         }
         file.close();
      };
      void StrManager::WriteFile() const {
         std::string path;
         {
            std::string	runtimePath = GetOblivionDirectory();
            if (runtimePath.empty())
               return;
            path = runtimePath;
            path += "Data\\OBSE\\Plugins\\NorthernUI.l10n.txt";
         }
         std::fstream file;
         file.open(path, std::ios_base::in);
         if (file) {
            _MESSAGE("The L10n file already exists and will not be written to."); // TODO: ADD MISSING KEYS TO THE FILE INSTEAD
            file.close();
            return;
         }
         file.open(path, std::ios_base::out | std::ios_base::trunc);
         if (!file) {
            _MESSAGE("Unable to open L10n file for reading or working file for writing.");
            return;
         }
         file.write(c_defaultFileBlurb.c_str(), c_defaultFileBlurb.size());
         std::vector<std::string> keys;
         this->GetSortedKeys(keys);
         for (auto it = keys.begin(); it != keys.end(); ++it) {
            auto entry = this->strings.at(*it);
            std::string line = *it;
            line += '=';
            if (entry->value) {
               std::string v = entry->value;
               cobb::replace_all_of(v, "\n", "\\n");
               line += v;
            }
            line += '\n';
            file.write(line.c_str(), line.size());
         }
         /*for (auto it = this->strings.begin(); it != this->strings.end(); ++it) {
            std::string line = it->first;
            line += '=';
            if (it->second->value)
               line += it->second->value;
            line += '\n';
            file.write(line.c_str(), line.size());
         }*/
         _MESSAGE("Wrote the L10n file.");
      };
   }
};