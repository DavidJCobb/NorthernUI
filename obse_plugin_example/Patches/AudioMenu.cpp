#include "AudioMenu.h"
#include "ReverseEngineered/UI/Menus/AudioMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "Services/Translations.h"
#include "obse_common/SafeWrite.h"
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"

namespace CobbPatches {
   namespace AudioMenu {
      namespace ResetDefaultsConfirmation {
         //
         // Add a confirmation box to resetting the defaults.
         //
         void Callback() {
            auto m = (RE::TileMenu*) g_TileMenuArray->data[RE::AudioMenu::kID - kMenuType_Message];
            if (!m)
               return;
            auto menu = (RE::AudioMenu*) m->menu;
            if (!menu)
               return;
            int button = RE::GetAndResetMessageButtonPressed();
            if (button == 1)
               menu->ResetDefaults();
         };
         void Inner() {
            ShowMessageBox(
               NorthernUI::L10N::sResetOptionsToDefault.value,
               &Callback, 0,
               RE::GMST::sNo->s,
               RE::GMST::sYes->s,
               nullptr
            );
         };
         __declspec(naked) void Outer() {
            _asm {
               call Inner;
               mov  eax, 0x0059593B;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005957F6, (UInt32)&Outer);
            SafeWrite8  (0x005957FB, 0x90); // courtesy NOP
         };
      };
      //
      void Apply() {
         ResetDefaultsConfirmation::Apply();
      };
   };
};