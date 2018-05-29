#pragma once
#include "ReverseEngineered/UI/MenuTextInputState.h"
#include "obse/GameMenus.h"
#include "obse/NiTypes.h"

typedef NiVector3 NiPoint3;

namespace RE {
   //extern UInt32* const iRaceSexMenuGeneratedIDCounter = (UInt32*) 0x00B3B5D4; // tracks how many option tiles have been generated

   class RaceSexMenu : public Menu { // sizeof == 0x9B0
      public:
         RaceSexMenu();
         ~RaceSexMenu();

         Tile*    tileListWindow = nullptr; // 028 // ID #1 // list window
         Tile*    tileModelGrab  = nullptr; // 02C // ID #2 // player model grab area?
         Tile*    tilePlayerName = nullptr; // 030 // ID#10 // name entry textbox
         Tile*    tileListScrollbar;        // 034 // ID#20 // list scrollbar
         Tile*    tileListScrollThumb;      // 038 // ID#21 // list scrollbar thumb
         Tile*    tileButtonDone = nullptr; // 03C // ID#90 // the "done" button
         Tile*    tileGeneratedPaneMain;    // 040
         Tile*    tileGeneratedPaneRace;    // 044
         Tile*    tileGeneratedPaneHair;    // 048
         Tile*    tileGeneratedPaneFace;    // 04C
         Tile*    tileGeneratedPaneFaceShape; // 050
         Tile*    tileGeneratedPaneFaceTone;  // 054
         Tile*    tileGeneratedPaneFaceShapeGeneral;  // 058
         Tile*    tileGeneratedPaneFaceShapeBrow;     // 05C
         Tile*    tileGeneratedPaneFaceShapeCheeks;   // 060
         Tile*    tileGeneratedPaneFaceShapeChin;     // 064
         Tile*    tileGeneratedPaneFaceShapeEyes;     // 068
         Tile*    tileGeneratedPaneFaceShapeForehead; // 06C
         Tile*    tileGeneratedPaneFaceShapeJaw;      // 070
         Tile*    tileGeneratedPaneFaceShapeMouth;    // 074
         Tile*    tileGeneratedPaneFaceShapeNose;     // 078
         Tile*    tileGeneratedPaneFaceToneGeneral;   // 07C
         Tile*    tileGeneratedPaneFaceToneBeard;     // 080
         Tile*    tileGeneratedPaneFaceToneEyes;      // 084
         Tile*    tileGeneratedPaneFaceToneEyebrows;  // 088
         Tile*    tileGeneratedPaneFaceToneLips;      // 08C
         Tile*    tileGeneratedPaneFaceToneNose;      // 090
         Tile*    generatedOptionTiles[500]; // 094 // listed by ID trait value, zero-indiced // initialized to all zeroes using REP STOS
         TESForm* unk864 = 0;   // 864 // equipped item type when the menu is opened
         UInt8    unk868 = 1;   // 868
         UInt8    unk869[3];
         UInt32   unk86C = 0;   // 86C
         UInt32   unk870 = 0;   // 870
         float    unk874 = 0.0; // 874
         UInt32   unk878 = 0;   // 878
         UInt32   unk87C = 0;   // 87C
         float    unk880 = 0.0; // 880
         float    unk884 = 0.0; // 884
         UInt32   unk888;
         UInt32   unk88C;
         UInt32   unk890;
         UInt8    unk894 = 0;   // 894
         UInt8    unk895[3];
         float    unk898 = 0.0; // 898 // == (tileX - cursorX) - tileWidth, when the menu first receives a mousedown
         float    unk89C;       // 89C // == similar to unk898, but for the Y-coordinate
         float    unk8A0 = 0.0; // 8A0 // related to X-coordinate; meaning unknown
         NiPoint3 unk8A4;       // 8A4
         UInt32   unk8B0;       // 8B0
         NiPoint3 unk8B4;       // 8B4
         NiPoint3 unk8C0;       // 8C0
         float    unk8CC = 0.0; // 8CC
         UInt8    unk8D0 = 0;   // 8D0
         UInt8    unk8D1;
         UInt8    unk8D2;
         UInt8    unk8D3;
         UInt32   unk8D4;       // 8D4
         UInt32   unk8D8;       // 8D8
         float    unk8DC = 0.0; // 8DC
         float    unk8E0 = 0.0; // 8E0
         UInt32   unk8E4;
         UInt32   unk8E8;
         MenuTextInputState* playerNameInputState; // 8EC
         UInt32   hairColors[16]; // 8F0
         char     hairColorNames[16][8]; // 930

         // 8F0 -- iHairColor00
         // 8F4 -- iHairColor01
         // 8F8 -- iHairColor02
         // 8FC -- iHairColor03
         // ...
         // 914 -- iHairColor09
         // ...
         // 92C -- iHairColor15
         // 930 -- char sHairColor0[8]
         // 938 -- char sHairColor1[8]
         // ...
         // 998 -- char sHairColor13[8]
         // 9A0 -- char sHairColor14[8]
         // 9A8 -- char sHairColor15[8]
   };
   static_assert(sizeof(RaceSexMenu) <= 0x9B0, "RE::RaceSexMenu is too large!");
   static_assert(sizeof(RaceSexMenu) >= 0x9B0, "RE::RaceSexMenu is too small!");
   static_assert(offsetof(RaceSexMenu, generatedOptionTiles) == 0x094, "RE::RaceSexMenu::generatedOptionTiles is in the wrong place!");
   static_assert(offsetof(RaceSexMenu, unk864) == 0x864, "RE::RaceSexMenu::unk864 is in the wrong place!");
   static_assert(offsetof(RaceSexMenu, unk898) == 0x898, "RE::RaceSexMenu::unk898 is in the wrong place!");
};