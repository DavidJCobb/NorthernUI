#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

class TESObjectREFR;
class TESSkill;
namespace RE {
   class TrainingMenu : public RE::Menu { // sizeof == 0x64
      public:
         enum {
            kID   = 0x404,
            kVTBL = 0x00A6DC74,
         };
         enum {
            kRootTrait_Gold = kTagID_user0, // string includes the label
         };
         enum {
            kTileID_MenuBackground   =  1,
            kTileID_SkillName        =  2,
            kTileID_SkillPicture     =  3,
            kTileID_SkillDescription =  4,
            kTileID_ButtonTrain      =  6,
            kTileID_ButtonExit       =  7,
            kTileID_TrainingCost     =  8,
            kTileID_TrainerLevelText =  9,
            kTileID_WarningText      = 10,
            kTildID_PlayerLevelText  = 11,
            kTileID_TimesTrainedText = 12,
         };

         Tile*  tileBackground       = nullptr; // 28
         Tile*  tileSkillName        = nullptr; // 2C
         Tile*  tileSkillPicture     = nullptr; // 30
         Tile*  tileSkillDescription = nullptr; // 34
         Tile*  tileButtonTrain      = nullptr; // 38 // disabled if: insufficient gold. hidden if: player has trained max times for this level already; player skill too high for this trainer
         Tile*  tileButtonExit       = nullptr; // 3C
         Tile*  tileTrainingCost     = nullptr; // 40
         Tile*  tileTrainerLevelText = nullptr; // 44
         Tile*  tileWarningText      = nullptr; // 48 // shows a message if the player has maxed training for this level, or if the player has outleveled this trainer
         Tile*  tilePlayerLevelText  = nullptr; // 4C
         Tile*  tileTimesTrainedText = nullptr; // 50
         TESObjectREFR* unk54; // 54
         TESSkill* trainingSkill; // 58
         UInt32 trainingCost;  // 5C
         UInt32 trainingLevel; // 60 // the trainer's training level; if the player exceeds this, they cannot train
   };
};