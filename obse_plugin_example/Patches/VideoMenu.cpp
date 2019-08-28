#include "VideoMenu.h"
#include "ReverseEngineered/UI/Menus/VideoMenu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/GameSettings.h"
#include "Services/Translations.h"
#include "obse_common/SafeWrite.h"
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"

namespace CobbPatches {
   namespace VideoMenu {
      namespace ResetDefaultsConfirmation {
         //
         // Add a confirmation box to resetting the defaults.
         //
         void Callback() {
            auto m = (RE::TileMenu*) g_TileMenuArray->data[RE::VideoMenu::kID - kMenuType_Message];
            if (!m)
               return;
            auto menu = (RE::VideoMenu*) m->menu;
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
               mov  eax, 0x005E010D;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005DFE72, (UInt32)&Outer);
            SafeWrite8  (0x005DFE77, 0x90); // courtesy NOP
            SafeWrite16 (0x005DFE78, 0x90); // courtesy NOPs
         };
      };
      //
      void Apply() {
         ResetDefaultsConfirmation::Apply();
      };
   };
};