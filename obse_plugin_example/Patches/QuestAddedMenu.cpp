#pragma once
#include "obse_common/SafeWrite.h"
#include "Services/INISettings.h"
#include "ReverseEngineered/Forms/PlayerCharacter.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameMenus.h"
#include "obse/Script.h"

namespace CobbPatches {
   namespace QuestAddedMenu {
      bool ShouldShowQuest(QuestStageItem* stage) { // if it returns false, the quest popup is suppressed
         if (!NorthernUI::INI::Features::bSuppressDLCStartup.bCurrent)
            return true;
         TESQuest* quest = stage->owningQuest;
         if (!quest)
            return true;
         auto stageIndex = quest->stageIndex;
         auto name = quest->editorName.m_data;
         if (!name)
            return true;
         if (stageIndex == 30 && strcmp(name, "DLCHorseArmor") == 0)
            return false;
         if (stageIndex == 10 && strcmp(name, "DLCFrostcragSpire") == 0)
            return false;
         if (stageIndex == 10 && strcmp(name, "DLC06ThievesDen") == 0)
            return false;
         if (stageIndex == 10 && strcmp(name, "DLCDeepscorn") == 0)
            return false;
         if (stageIndex == 10 && strcmp(name, "DL9MehrunesQuest") == 0)
            return false;
         if (stageIndex == 10 && strcmp(name, "DLCBattlehornCastle") == 0)
            return false;
         if (stageIndex == 10 && strcmp(name, "DLCOrrery") == 0)
            return false;
         return true;
      };
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
      bool Inner(RE::PlayerCharacter* player, QuestStageItem* stage) {
         if (!ShouldShowQuest(stage))
            return false;
         if (CALL_MEMBER_FN(player, ShowQuestUpdate)(stage)) {
            TamperWithTutorialHeader(stage);
            return true;
         }
         return false;
      };
      __declspec(naked) void Outer(QuestStageItem* stage) {
         _asm {
            mov  eax, dword ptr [esp + 0x4];
            push eax;
            push ecx; // protect
            call Inner;
            add  esp, 8;
            retn 4;
         };
      };

      void Apply() {
         WriteRelCall(0x0052AEEE, (UInt32)&Outer);
      };
   };
};