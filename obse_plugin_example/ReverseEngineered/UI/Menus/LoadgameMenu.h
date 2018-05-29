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
   class LoadgameMenu : public Menu { // sizeof == 0x68
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
         Tile*  unk44                 = nullptr; // 44 // ID #7 // load_Text
         Tile*  tileFileListPane      = nullptr; // 48 // ID #9
         SaveGameFile* fileSlatedForDeletion = nullptr; // 4C // set just before we prompt the user for confirmation
         float         dragStartYOffset      = 0.0;     // 50 // When the mouse goes down, this is set to the clicked tile's absolute Y-coordinate minus the cursor Y.
         LinkedPointerList<SaveGameFile>* unk54 = nullptr; // 54 // LinkedPointerList<????>*
         Tile*     unk58 = nullptr; // 58 // save file we're prompting for deletion
         BSStringT unk5C;           // 5C // set to the user3 of the file being deleted when deletion is confirmed; conditional; don't understand the conditions. so, when this is actually checked, it would hold the name of the last deleted file?
         bool   awaitingMessageBoxCallback = false; // 64
         UInt8  pad65;
         UInt8  pad66;
         UInt8  pad67;

         // IDs starting from 101 are generated list items.

         MEMBER_FN_PREFIX(LoadgameMenu);
         DEFINE_MEMBER_FN(PostDeletionHandler, void,  0x005AE980); // called after a save file is deleted
         DEFINE_MEMBER_FN(RenderSavegame,      Tile*, 0x005AE6D0, char*, UInt32 listindex, SaveGameFile*, UInt32); // pass a nullptr save file to show "No saved games."
   };

   extern _ShowMessageBox_Callback const LoadGameMenu_DeleteSaveFileConfirmationCallback; // TODO: member function with two args
   extern _ShowMessageBox_Callback const LoadGameMenu_LoadSaveFileConfirmationCallback;

   // subroutine at 005AE430 as well
};