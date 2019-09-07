#pragma 
#include "ReverseEngineered/_BASE.h"
#include "ReverseEngineered/UI/Menu.h"
#include "ReverseEngineered/UI/Tile.h"
#include "obse/GameAPI.h"

namespace RE {
   class PauseMenu : public Menu { // sizeof == 0x3C
      public:
         Tile* unk28 = nullptr; // 28 // tile ID #3
         Tile* unk2C = nullptr; // 2C // tile ID #4
         Tile* unk30 = nullptr; // 30 // tile ID #5
         Tile* unk34 = nullptr; // 34 // tile ID #7
         Tile* unk38 = nullptr; // 38 // tile ID #8
   };

   DEFINE_SUBROUTINE_EXTERN(void, ClosePauseMenu,           0x005BDCD0);
   DEFINE_SUBROUTINE_EXTERN(void, ShowPauseMenuIfPermitted, 0x0057B560);
};