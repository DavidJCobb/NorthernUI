#include "Patches/SpellMakingMenu.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/SpellMakingMenu.h"
#include "obse/GameObjects.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace SpellMakingMenu {
      namespace WriteSkillRequirement {
         void _stdcall Inner(RE::SpellMakingMenu* menu) {
            auto tile = (RE::Tile*) menu->tile;
            if (!tile)
               return;
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user22, -1.0F);
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user23, -1.0F);
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user24, -1.0F);
            auto spell = menu->unk74;
            if (!spell)
               return;
            auto list    = (RE::EffectItemList*)&(spell->magicItem.list);
            auto effect  = CALL_MEMBER_FN(list, GetCostliestEffect)(3, false);
            auto school  = CALL_MEMBER_FN(effect, GetMagicSchool)();
            auto avIndex = RE::MagicSchoolToActorValueIndex(school);
            if (avIndex < 0)
               return;
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user22, avIndex);
            auto requiredSkill = RE::SkillMasteryLevelToMinimumSkillLevel(list->GetMasteryLevel());
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user23, requiredSkill);
            auto playerSkill = (*g_thePlayer)->GetBaseActorValue(avIndex);
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_user24, playerSkill);
         };
         __declspec(naked) void Outer() {
            _asm {
               mov  eax, 0x0058CED0; // reproduce patched-over call to Tile::UpdateString
               call eax;             //
               push edi;   //
               call Inner; // stdcall
               mov  ecx, 0x005D8592;
               jmp  ecx;
            };
         };
         void Apply() {
            WriteRelJump(0x005D858D, (UInt32)&Outer);
         };
      };
      //
      void Apply() {
         WriteSkillRequirement::Apply();
      };
   };
};