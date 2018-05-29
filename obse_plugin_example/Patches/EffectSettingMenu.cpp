#include "EffectSettingMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameMenus.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace EffectSettingMenu {
      namespace SkillsMenuAware {
         void Inner(RE::Tile* root) {
            auto tileSkills = (RE::Tile*) g_TileMenuArray->data[kMenuType_Skills - kMenuType_Message];
            bool skillsOpen = false;
            if (tileSkills && CALL_MEMBER_FN(tileSkills, GetFloatTraitValue)(kTileValue_visible) != 1.0F)
               skillsOpen = true;
            //
            CALL_MEMBER_FN(root, UpdateFloat)(kTileValue_user22, skillsOpen ? 2.0F : 1.0F);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  ebp, 2; // patched-over instruction
               mov  ecx, dword ptr [esi + 0x4];
               push ecx;
               call Inner;
               add  esp, 4;
               mov  eax, 0x005A0277;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005A0272, (UInt32)&Outer);
         };
      };
      namespace ShowChargeCostBetter {
         //
         // The vanilla menu shows you the magicka/charge cost of the spell you're crafting, and 
         // the max possible cost you can craft. However, it stringifies them as "%d (%d)". This 
         // patch supplies the two numbers individually.
         //
         void Inner(RE::Tile* root, SInt32 current, SInt32 maxPossible) {
            CALL_MEMBER_FN(root, UpdateFloat)(kTileValue_user23, current);
            CALL_MEMBER_FN(root, UpdateFloat)(kTileValue_user24, maxPossible);
         };
         __declspec(naked) void JustCurrent_Outer() {
            _asm {
               mov  eax, 0x0058CED0; // reproduce patched-over Tile::UpdateString call
               call eax;
               //
               fld  dword ptr [esp + 0x10];
               mov  eax, 0x009828C0;
               call eax; // SInt32 eax = DoubleToSInt32();
               push -1;
               push eax;
               mov  ecx, dword ptr [esi + 0x4];
               push ecx;
               call Inner;
               add  esp, 0xC;
               //
               mov  eax, 0x005A057B;
               jmp  eax;
            };
         };
         __declspec(naked) void OutOfMax_Outer() {
            _asm {
               mov  eax, 0x0058CED0; // reproduce patched-over Tile::UpdateString call
               call eax;
               //
               push ebp;
               push edi;
               mov  ecx, dword ptr [esi + 0x4];
               push ecx;
               call Inner;
               add  esp, 0xC;
               //
               mov  eax, 0x005A0797;
               jmp  eax;
            };
         };
         void Apply() {
            WriteRelJump(0x005A0576, (UInt32)&JustCurrent_Outer);
            WriteRelJump(0x005A0792, (UInt32)&OutOfMax_Outer);
         };
      };

      void Apply() {
         SkillsMenuAware::Apply();
         ShowChargeCostBetter::Apply();
      };
   };
};