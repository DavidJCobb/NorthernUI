#include "GameplayMenu.h"
#include "ReverseEngineered/UI/Menus/GameplayMenu.h"
#include "ReverseEngineered/GameSettings.h"
#include "Services/Translations.h"
#include "obse_common/SafeWrite.h"
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"

namespace CobbPatches {
   namespace GameplayMenu {
      namespace ResetDefaultsConfirmation {
         //
         // Add a confirmation box to resetting the defaults.
         //
         void Callback() {
            auto m = (RE::TileMenu*) g_TileMenuArray->data[RE::GameplayMenu::kID - kMenuType_Message];
            if (!m)
               return;
            auto menu = (RE::GameplayMenu*) m->menu;
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
               mov  eax, 0x005A3B61;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005A3A21, (UInt32)&Outer);
            SafeWrite8  (0x005A3A26, 0x90); // courtesy NOP
         };
      };
      namespace ReturnAlphaFix {
         void Apply() {
            //
            // GameplayMenu sets the ALPHA trait on its "Return" button for no real reason that 
            // I can discern. This accomplishes absolutely nothing in most cases, but can cause 
            // issues for displaying the button in other cases.
            //
            // It's not even used for semitranslucency. It's set to either 0 or 255. There's 
            // absolutely no reason they couldn't have used XML for this if they needed it, which 
            // they didn't.
            //
            // No-op GameplayMenu::HandleMouseout
            //
            SafeWrite8(0x005A3BB0 + 0, 0xC2); // 
            SafeWrite16(0x005A3BB0 + 1, 8);    // RETN 8
            SafeWrite16(0x005A3BB0 + 3, 0x9090);
            //
            // No-op GameplayMenu::HandleMouseover
            //
            SafeWrite8(0x005A3B90 + 0, 0xC2); // 
            SafeWrite16(0x005A3B90 + 1, 8);    // RETN 8
            SafeWrite16(0x005A3B90 + 3, 0x9090);
         };
      };
      //
      void Apply() {
         ResetDefaultsConfirmation::Apply();
         ReturnAlphaFix::Apply();
      };
   };
};