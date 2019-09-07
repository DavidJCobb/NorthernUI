#pragma 
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/TESSaveLoadGame.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameAPI.h"

namespace RE {
   //
   // Actually, LoadgameMenu and SaveMenu both inherit from another Menu subclass.
   //
   class SaveMenu : public Menu { // sizeof == 0x60
      public:
         Tile*  tileButtonExit = nullptr; // 28 // ID #1
         Tile*  tileFocusBox   = nullptr; // 2C // ID #2
         UInt32 unk30          = 0;       // 30
         Tile*  tileScrollbar         = nullptr; // 34 // ID #3
         Tile*  tileScrollThumb       = nullptr; // 38 // ID #4
         Tile*  tileFileListContainer = nullptr; // 3C // ID #5
         Tile*  tileFileThumbnail     = nullptr; // 40 // ID #6 // game dynamic-casts this to TileImage sometimes
         //
         // Fields after this point are inconsistent between SaveMenu and LoadgameMenu.
         //
         UInt32 unk44;
         UInt32 unk48;
         UInt32 unk4C;
         BSStringT unk50; // 50
         UInt8     unk5C; // 5C
         UInt8     pad5D[3];
   };
};