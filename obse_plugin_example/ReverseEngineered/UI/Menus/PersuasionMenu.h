#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

namespace RE {
   class Actor;
   class PersuasionMenu : public Menu { // sizeof == 0xFC
      public:
         enum {
            kVTBL = 0x00A6D0CC,
         };
         enum {
            kTileID_GradientImage =  1,
            kTileID_UnknownID_4   =  4,
            kTileID_BribeReadout  =  5,
            kTileID_UnknownID_6   =  6,
            kTileID_ButtonBribe   =  8,
            kTileID_ButtonExit    =  9,
            kTileID_Disposition   = 10,
            kTileID_ReactionText  = 11,
            kTileID_PotentialIncrease = 15,
            kTileID_Gradient_025  = 16,
            kTileID_Gradient_050  = 17,
            kTileID_Gradient_075  = 18,
            kTileID_Gradient_100  = 19,
            kTileID_Dimmer1       = 20,
            kTileID_Dimmer2       = 21,
            kTileID_Dimmer3       = 22,
            kTileID_Dimmer4       = 23,
            kTileID_ButtonRotate  = 24,
            kTileID_UnknownID_25  = 25,
            kTileID_ButtonStart   = 26,
            kTileID_CantPersuade  = 27,
            kTileID_NPCName       = 28,
         };

         struct Quadrant { // sizeof == 0x14
            SInt32 unk00;  // 00 (2C, 40, 54, 68) // quadrant's current offset/position/whatever as an index, i.e. 1, 2, 3, 4?
            UInt32 unk04;  // 04 (30, 44, 58, 6C) // initialized to quadrant's index?
            UInt32 unk08;
            UInt32 unk0C;
            Tile*  dimmer; // 00 (3C, 50, 64, 78)
         };

         UInt32 unk28; // if this == 2, then the minigame is active
         Quadrant quadrants[4]; // 2C, 40, 54, 68
         UInt32 unk7C;
         UInt32 unk80; // 80 // milliseconds
         UInt32 unk84; // 84 // quadrant we're currently aiming at, by index i.e. 1, 2, 3, 4?
         UInt32 unk88;
         UInt8  unk8C; // 8C
         UInt8  unk8D;
         UInt8  unk8E;
         UInt8  unk8F;
         Tile*  tilePotentialIncrease = nullptr; // 90
         Tile*  tileCantPersuadeText  = nullptr; // 94
         Tile*  tileGradient025 = nullptr; // 98
         Tile*  tileGradient050 = nullptr; // 9C
         Tile*  tileGradient075 = nullptr; // A0
         Tile*  tileGradient100 = nullptr; // A4
         Tile*  tileMinigame3D  = nullptr; // A8
         Tile*  tileID4 = nullptr; // AC
         Tile*  tileBribeReadout = nullptr; // B0
         Tile*  tileID6 = nullptr; // B4
         Tile*  tileButtonBribe = nullptr; // B8
         Tile*  tileButtonExit = nullptr; // BC
         Tile*  tileButtonRotate = nullptr; // C0
         Tile*  tileButtonStart  = nullptr; // C4
         Tile*  tileID25 = nullptr; // C8
         Tile*  tileDisposition = nullptr; // CC
         Tile*  tileReactionText = nullptr; // D0
         Tile*  tileNPCName = nullptr; // D4
         Actor* targetNPC = nullptr; // D8
         SInt32 minigameOuterRadius;  // DC // initialized to tileMinigame3D user7 // mouse must be within the outer radius and outside of the inner radius
         SInt32 minigameCenterpointX; // E0 // initialized to tileMinigame3D user8
         SInt32 minigameCenterpointY; // E4 // initialized to tileMinigame3D user9
         SInt32 minigameInnerRadius;  // E8 // initialized to tileMinigame3D user6
         UInt32 unkEC;
         UInt32 unkF0;
         UInt32 unkF4;
         UInt32 unkF8;

         MEMBER_FN_PREFIX(PersuasionMenu);
         DEFINE_MEMBER_FN(MouseCoordinatesToQuadrantIndex, UInt32, 0x005BE6F0, float x, float y, float unusedZ);
   };
   static_assert(sizeof(PersuasionMenu) <= 0xFC, "RE::PersuasionMenu is too large!");
   static_assert(sizeof(PersuasionMenu) >= 0xFC, "RE::PersuasionMenu is too small!");
   static_assert(offsetof(PersuasionMenu, tilePotentialIncrease) >= 0x90, "RE::PersuasionMenu::tilePotentialIncrease is too early!");
   static_assert(offsetof(PersuasionMenu, tilePotentialIncrease) <= 0x90, "RE::PersuasionMenu::tilePotentialIncrease is too late!");
   static_assert(offsetof(PersuasionMenu, targetNPC) >= 0xD8, "RE::PersuasionMenu::targetNPC is too early!");
   static_assert(offsetof(PersuasionMenu, targetNPC) <= 0xD8, "RE::PersuasionMenu::targetNPC is too late!");
   static_assert(offsetof(PersuasionMenu, minigameOuterRadius) >= 0xDC, "RE::PersuasionMenu::minigameOuterRadius is too early!");
   static_assert(offsetof(PersuasionMenu, minigameOuterRadius) <= 0xDC, "RE::PersuasionMenu::minigameOuterRadius is too late!");
};