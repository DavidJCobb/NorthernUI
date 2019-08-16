#pragma once
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"

class Character;
namespace RE {
   class DialogMenu : public Menu {
      public:
         DialogMenu();
         ~DialogMenu();

         enum {
            kTileID_TopicsList    = 1,
            kTileID_TopicsWrapper = 6,
            kTileID_ButtonBarter  = 8,
            kTileID_NorthernUI_ButtonBarterShim = 9001,
         };

         Tile* tileTopicScrollPane;		//028 // ID#1
         Tile* tileDialogText;			//02C
         Tile* tileButtonGoodbye;		//030
         Tile* tileDialogTextLayout;		//034
         Tile* tileFocusBox;				//038
         Tile* tileTopics;				//03C // ID#6
         Tile* tileButtonPersuade;			//040
         Tile* tileScrollBar;			//044
         Tile* tileScrollMarker;			//048
         Tile* tileButtonBarter; // 4C
         Tile* tileButtonTrain;			//050
         Tile* tileButtonRecharge;		//054
         Tile* tileButtonRepair;			//058
         Tile* tileButtonSpells;			//05C
         Character* speaker;		//060
         UInt32				unk064;			//064
         UInt32				unk068;			//068
         UInt32				unk06C;			//06C init'd to 1
         UInt32				unk070;			//070
         UInt32				unk074;			//074
         UInt32				unk078;			//078
         bool    wasThirdPersonView; // 7C // was the player in third-person view when they first entered dialogue?
         UInt8				pad07D[3];
         UInt32				unk080;			//080 init'd to 1
         UInt32				unk084;			//084
         UInt8				unk088;			//088
         UInt8				pad087[3];
         BSStringT				str08C;			//08C
         UInt8				unk094[3];		//094
         UInt8				pad097;

         MEMBER_FN_PREFIX(DialogMenu);
         DEFINE_MEMBER_FN(RenderTopics, bool, 0x0056E680);
   };
};