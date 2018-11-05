#pragma once
#include <unordered_map>

namespace NorthernUI {
   namespace L10N {
      struct Str;

      extern Str const sNotImplemented;
      extern Str const sOrdinalSuffixLower0;
      extern Str const sOrdinalSuffixLower1;
      extern Str const sOrdinalSuffixLower2;
      extern Str const sOrdinalSuffixLower3;
      extern Str const sOrdinalSuffixLower4;
      extern Str const sOrdinalSuffixUpper0;
      extern Str const sOrdinalSuffixUpper1;
      extern Str const sOrdinalSuffixUpper2;
      extern Str const sOrdinalSuffixUpper3;
      extern Str const sOrdinalSuffixUpper4;
      extern Str const sTimeSuffixLowerAM;
      extern Str const sTimeSuffixLowerPM;
      extern Str const sTimeSuffixUpperAM;
      extern Str const sTimeSuffixUpperPM;
      extern Str const sControlsKeyConflict;
      extern Str const sControlsKeyConflictConfirm;
      extern Str const sControlsSwitchSchemeUnsaved;
      extern Str const sControlsDefaultProfileName;
      extern Str const sControlsDefaultNewProfileName;
      extern Str const sControlsExitUnsaved;
      extern Str const sControlsExitUnsavedCancel;
      extern Str const sControlsExitUnsavedNoSave;
      extern Str const sControlsExitUnsavedSave;
      extern Str const sControlsExitUnsavedSaveAsNew;
      extern Str const sControlsDelete;
      extern Str const sControlsExitSaveAsNewNameTaken;
      extern Str const sControlsCreateNamePrompt;
      extern Str const sControlsRenamePrompt;
      extern Str const sControlsNameCannotBeBlank;
      extern Str const sControlsNameIsTaken;
      extern Str const sControlsSaveAsNewButItsTheSameName;
      extern Str const sControlsCreateNameTaken;
      extern Str const sControlsCreateUnsaved;
      extern Str const sControlsCreateSaveFirst;
      extern Str const sControlsCreateDiscard;
      extern Str const sResetOptionsToDefault;
      extern Str const sBadMenuQueVersion;

      class StrManager {
         private:
            std::unordered_map<std::string, Str*> strings;
            void GetSortedKeys(std::vector<std::string>& out) const;
            //
         public:
            static StrManager& GetInstance();
            //
            void Add(const char* name, Str* value);
            void Update();
            void WriteFile() const;
      };

      struct Str {
         const char* const label;
         char*             value; // forgoing std::string for efficiency's sake, since we have stuff that runs on every frame
         const char* const tokens = nullptr; // validation for format strings, or nullptr if no tokens, though Str itself doesn't perform validation

         Str(const char* a, const char* b);
         Str(const char* a, const char* b, const char* c);
         void Set(const char*);
         void Set(std::string&);
      };

      void Update();

      // blurb written to the start of L10n file if it's missing and we're regenerating it:
      const std::string c_defaultFileBlurb = \
         "; \n"\
         "; This file contains strings that need to be translatable, but also available \n"\
         "; to the DLL directly. Only strings hardcoded into the DLL will be recognized \n"\
         "; (which makes sense, since the DLL only uses what it, uh, uses).\n"\
         "; \n"\
         "; Some of these strings have printf formatting codes, e.g. %s. If for some  \n"\
         "; reason you need to put actual percent signs in these strings, double them  \n"\
         "; e.g. \"%%\". Ensure that the printf formatting codes aren't changed; they  \n"\
         "; aren't checked by the executable for correctness, and crashes will result  \n"\
         "; if they are damaged. \n"\
         ";";
   };
};

class NorthernUITranslations {
   private:
      //
      // The translations map should use const char* values because we need to be able to 
      // return raw pointers to clients. Why? At least one client runs every frame, and we 
      // don't want it to have to query the full unordered map every frame; it's better if 
      // it keeps static variables pointing to the translated strings it needs, but that's 
      // only possible if we manage the memory ourselves. If we store std::strings and just 
      // return c_str(), the underlying strings can get moved and clients will (in the best 
      // case) get corrupt strings from stale static-variable pointers.
      //
      std::unordered_map<std::string, const char*> translations;
      //
      NorthernUITranslations();
      ~NorthernUITranslations();
   public:
      static NorthernUITranslations& GetInstance();
      //
      const char* GetTranslation(const char* key);
};