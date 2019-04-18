#include "Main.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/UI/TagIDs.h"
#include "ReverseEngineered/UI/Tile.h"
#include "ReverseEngineered/UI/Menus/TrainingMenu.h"
#include "obse/GameData.h"
#include "obse_common/SafeWrite.h"

namespace CobbPatches {
   namespace DynamicTrainingCost {
      //
      // Allow TrainingMenu XML to "see" the state of Dynamic Training Cost 
      // script variables, so we can adjust layout accordingly.
      //
      enum {
         kRootTileTrait_TrainingTimeString    = kTileValue_user22,
         kRootTileTrait_TrainingTakesTime     = kTileValue_user23,
         kRootTileTrait_DisplayingAdjustments = kTileValue_user24,
      };

      struct _State {
         inline static _State& GetInstance() {
            static _State instance;
            return instance;
         };
         //
         _State() {
            this->quest = (*g_dataHandler)->GetQuestByEditorName("migTrainingQ", strlen("migTrainingQ"));
         }
         //
         TESQuest* quest = nullptr;
         //
         // Dynamic Training Cost has a feature to make training take time. It displays the 
         // time to train by appending it to the displayed player skill level, separated 
         // with two '\n'. We want to separate that out onto its own row, since we right-
         // align the various training stats and line breaks don't align properly.
         //
         bool        trainingTakesTime = false;
         std::string tamperedPlayerSkill;
      };

      namespace OnFrame {
         static uint32_t original_frame_handler = 0;

         void _stdcall Inner(RE::TrainingMenu* menu) {
            auto& state = _State::GetInstance();
            TESQuest* quest = state.quest;
            if (!quest)
               return;
            auto& existing = state.tamperedPlayerSkill;
            if (!existing.empty())
               //
               // MenuQue executes its "OnMenuOpened" script hook at some point during or immediately 
               // after the menu open process, so we only actually need to do anything for the first 
               // frame.
               //
               return;
            if (state.trainingTakesTime) {
               auto root = menu->tile;
               auto tile = RE::GetDescendantTileByName(root, "Player_Level_Text"); // not quite exact, but close enough
               if (tile) {
                  existing = CALL_MEMBER_FN(tile, GetStringTraitValue)(kTileValue_string);
                  //
                  auto index = existing.find("\n\n");
                  if (index != std::string::npos) {
                     std::string skill = existing.substr(0, index);
                     std::string timer = existing.substr(index + 2);
                     CALL_MEMBER_FN(root, UpdateString)(kRootTileTrait_TrainingTimeString, timer.c_str());
                     CALL_MEMBER_FN(tile, UpdateString)(kTileValue_string, skill.c_str());
                     existing = skill;
                  }
               }
            }
         }
         __declspec(naked) void Outer() {
            _asm {
               push ecx; // protect
               push ecx;
               call Inner; // stdcall
               pop  ecx; // restore
               mov  eax, original_frame_handler;
               test eax, eax;
               jz   lExit;
               call eax;
            lExit:
               retn;
            }
         }
         void Apply() {
            uint32_t p = (RE::TrainingMenu::kVTBL + (4 * 0xB));
            //
            original_frame_handler = *(uint32_t*)p;
            SafeWrite32(p, (UInt32)&Outer);
         }
      }
      namespace OnMenuOpen {
         void _stdcall Inner(RE::TrainingMenu* menu) {
            auto& state = _State::GetInstance();
            TESQuest* quest = state.quest;
            if (!quest)
               return;
            state.tamperedPlayerSkill = ""; // reset this when the menu opens
            double varValue;
            if (RE::GetScriptVariableValue(quest, "bDisplayTrainAdjust", varValue)) {
               float result = RE::kEntityID_false;
               if (varValue > 0.0F)
                  if (RE::GetScriptVariableValue(quest, "display", varValue) && varValue != 0.0F)
                     result = RE::kEntityID_true;
               CALL_MEMBER_FN(menu->tile, UpdateFloat)(kRootTileTrait_DisplayingAdjustments, result);
            }
            if (RE::GetScriptVariableValue(quest, "bTrainTakesTime", varValue)) {
               state.trainingTakesTime = varValue > 0.0F;
               float result = state.trainingTakesTime ? RE::kEntityID_true : RE::kEntityID_false;
               CALL_MEMBER_FN(menu->tile, UpdateFloat)(kRootTileTrait_TrainingTakesTime, result);
            } else
               CALL_MEMBER_FN(menu->tile, UpdateFloat)(kRootTileTrait_TrainingTakesTime, RE::kEntityID_false);
         }
         __declspec(naked) void Outer() {
            _asm {
               push ecx; // protect
               push esi;
               call Inner; // stdcall
               pop  ecx; // restore
               mov  eax, 0x005E02E0; // reproduce patched-over call to TESObjectREFR::GetBaseOrTemplatedActor
               call eax;             //
               mov  ecx, 0x005DD5C7;
               jmp  ecx;
            }
         }
         void Apply() {
            WriteRelJump(0x005DD5C2, (UInt32)&Outer);
         }
      }
      //
      void Apply() {
         OnMenuOpen::Apply();
         OnFrame::Apply();
      }
   }
};