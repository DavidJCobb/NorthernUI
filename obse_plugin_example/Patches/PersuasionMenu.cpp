#include "PersuasionMenu.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Menus/PersuasionMenu.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace PersuasionMenu {
      namespace MinigameKeynav {
         //
         // Allow keyboard/gamepad navigation of the minigame itself, contingent on 
         // whether the root tile user24 is true.
         //
         bool InnerNavInput(RE::PersuasionMenu* menu, UInt32 navcode, float unkArg) {
            if (CALL_MEMBER_FN((RE::Tile*)menu->tile, GetFloatTraitValue)(kTileValue_user24) != 2.0F)
               return false;
            if (menu->unk28 != 2)
               return false;
            if (navcode == RE::InterfaceManager::kNavigationKeypress_XboxA) {
               //
               // Fire minigame click:
               //
               menu->HandleMouseUp(1, menu->tileMinigame3D);
               return true;
            }
            float halfwayRadius = ((float)menu->minigameOuterRadius - (float)menu->minigameInnerRadius) / 2 + menu->minigameInnerRadius;
            float xMod = 0;
            float yMod = 0;
            switch (navcode) {
               case RE::InterfaceManager::kNavigationKeypress_Up:
                  yMod = -1;
                  xMod = 0;
                  break;
               case RE::InterfaceManager::kNavigationKeypress_Down:
                  yMod = 1;
                  xMod = 0;
                  break;
               case RE::InterfaceManager::kNavigationKeypress_Left:
                  yMod = 0;
                  xMod = -1;
                  break;
               case RE::InterfaceManager::kNavigationKeypress_Right:
                  yMod = 0;
                  xMod = 1;
                  break;
               default:
                  return false;
            };
            float xCursor = menu->minigameCenterpointX;
            float yCursor = menu->minigameCenterpointY;
            if (xMod)
               xCursor += xMod * halfwayRadius;
            if (yMod)
               yCursor += yMod * halfwayRadius;
            RE::InterfaceManager::GetInstance()->SetCursorPosition(xCursor, yCursor);
         };
         __declspec(naked) void OuterNavInput(UInt32 navcode, float) {
            _asm {
               mov  edx, dword ptr [esp + 0x8];
               mov  eax, dword ptr [esp + 0x4];
               push edx;
               push eax;
               push ecx;
               call InnerNavInput;
               add  esp, 0xC;
               retn 8;
            };
         };
         void Apply() {
            SafeWrite32(RE::PersuasionMenu::kVTBL + (4 * 0xE), (UInt32)&OuterNavInput); // define PersuasionMenu::HandleNavigationInput
         };
      };

      void Apply() {
         MinigameKeynav::Apply();
      };
   };
};