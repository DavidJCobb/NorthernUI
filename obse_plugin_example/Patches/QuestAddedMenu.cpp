#pragma once
#include <array>
#include <cstdint>
#include "obse_common/SafeWrite.h"
#include "Services/INISettings.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameData.h"
#include "obse/GameMenus.h"
#include "obse/Script.h"

namespace {
   struct _quest {
      uint32_t    formID;
      uint8_t     stage;
      const char* name;
   };
   std::array<_quest, 7> _quests = {{
      { 0x000CEC, 30, "DLCHorseArmor" },
      { 0x000D53, 10, "DLCFrostcragSpire" },
      { 0x003E84, 10, "DLC06ThievesDen" },
      { 0x0041C4, 10, "DLCDeepscorn" },
      { 0x000ED9, 10, "DL9MehrunesQuest" },
      { 0x00B089, 10, "DLCBattlehornCastle" },
      { 0x11B9FB, 10, "DLCOrrery" },
   }};

   static bool s_postPatchFixup = false;
}

namespace CobbPatches {
   namespace QuestAddedMenu {
      namespace SuppressDLCSpam {
         bool ShouldShowQuest(QuestStageItem* stage) { // if it returns false, the quest popup is suppressed
            if (s_postPatchFixup)
               return false;
            if (!NorthernUI::INI::Features::bSuppressDLCStartup.bCurrent)
               return true;
            TESQuest* quest = stage->owningQuest;
            if (!quest)
               return true;
            auto stageIndex = quest->stageIndex;
            auto name = quest->editorName.m_data;
            if (!name)
               return true;
            uint32_t truncID = quest->refID & 0xFFFFFF;
            for (auto& q : _quests) {
               if (stageIndex != q.stage)
                  continue;
               if (strcmp(name, q.name) != 0)
                  continue;
               return false;
            }
            return true;
         }
         __declspec(naked) void Outer() {
            _asm {
               push esi;
               call ShouldShowQuest;
               add  esp, 4;
               mov  ecx, 0x0066984B;
               jmp  ecx;
            }
         }
         void Apply() {
            WriteRelJump(0x00669846, (UInt32)&Outer);
         }
      }
      namespace SuppressDLCSpamRetroactiveFix {
         //
         // Earlier versions of the SuppressDLCSpam patch accidentally prevented the spammy 
         // quest stages from even showing up in the journal. We want to retroactively fix 
         // that where possible.
         //
         void Execute() {
            auto* dh     = *g_dataHandler;
            auto* player = (RE::PlayerCharacter*)*g_thePlayer;
            if (!dh || !player)
               return;
            s_postPatchFixup = true;
            //
            auto& known      = *(RE::tList<QuestStageItem>*)&player->knownQuestStageItems;
            auto& all_quests = *(RE::tList<TESQuest>*)&dh->quests;
            int   found      = 0;
            int   fixed      = 0;
            for (auto* quest : all_quests) {
               if (!quest)
                  continue;
               _quest* match = nullptr;
               for (auto& entry : _quests) {
                  auto name = quest->editorName.m_data;
                  if (!name)
                     continue;
                  if (strcmp(name, entry.name) != 0)
                     continue;
                  match = &entry;
                  break;
               }
               if (!match)
                  continue;
               ++found;
               //
               {
                  bool abort = false;
                  for (auto* k : known) {
                     if (!k)
                        continue;
                     if (k->owningQuest == quest) {
                        abort = true;
                        break;
                     }
                  }
                  if (abort)
                     //
                     // Another stage from this quest is currently in the journal. If we try to 
                     // show the stage we accidentally omitted from the journal, we'll end up 
                     // yeeting the already-visible stage.
                     //
                     // If, for example, the player has progressed past the omitted stage and 
                     // a later stage is in the journal, trying to show the omitted stage will 
                     // hide the later stage and show the omitted stage -- even if the quest is 
                     // complete!
                     //
                     continue;
               }
               //
               auto& stages = *(RE::tList<TESQuest::StageEntry>*)&quest->stageList;
               for (auto* stage : stages) {
                  if (!stage || stage->index != match->stage)
                     continue;
                  auto& items = *(RE::tList<QuestStageItem>*)&stage->itemList;
                  for (auto* item : items) {
                     if (!item)
                        continue;
                     if (!item->logDate) // initialized when the game tries to show a stage in the journal, before the call that we had messed with in previous versions; thus a reliable indicator for stages that are supposed to be visible
                        continue;
                     ++fixed;
                     if (known.find(item) == known.end()) // make sure this stage hasn't already been shown
                        CALL_MEMBER_FN(player, ShowQuestUpdate)(item);
                     break;
                  }
                  break;
               }
               //
               if (found >= _quests.size())
                  break;
            }
            if (fixed) {
               _MESSAGE("QuestAddedMenu patch: performed post-patch fixup for the SuppressDLCSpam hook; this should remedy %d quest stages (of %d which may or may not be broken) that failed to ever show up in the journal.", fixed, found);
            }
            //
            s_postPatchFixup = false;
         }
      }
      namespace FixHeader {
         void TamperWithTutorialHeader(QuestStageItem* stage) {
            TESQuest* quest = stage->owningQuest;
            if (!quest)
               return;
            auto name = quest->editorName.m_data;
            if (!name)
               return;
            bool isTutorial = (strcmp(name, "MQ01") == 0);
            //
            auto root = (RE::Tile*) g_TileMenuArray->data[0x3F3 - kMenuType_Message]; // GenericMenu
            if (root)
               CALL_MEMBER_FN(root, UpdateFloat)(kTileValue_user24, isTutorial ? 2.0F : 1.0F);
         };
         void Inner(RE::PlayerCharacter* player, QuestStageItem* stage) {
            if (CALL_MEMBER_FN(player, ShowQuestUpdate)(stage)) {
               TamperWithTutorialHeader(stage);
            }
         }
         __declspec(naked) void Outer(QuestStageItem* stage) {
            _asm {
               mov  eax, dword ptr [esp + 0x4];
               push eax;
               push ecx;
               call Inner;
               add  esp, 8;
               retn 4;
            };
         }
         void Apply() {
            WriteRelCall(0x0052AEEE, (UInt32)&Outer); // shim a call to PlayerCharacter::ShowQuestUpdate
         }
      }
      //
      void Apply() {
         SuppressDLCSpam::Apply();
         FixHeader::Apply();
      }
   };
};