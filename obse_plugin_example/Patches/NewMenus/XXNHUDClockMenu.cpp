#include "XXNHUDClockMenu.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/Systems/Timing.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "obse/GameAPI.h"
#include "obse/GameData.h" // FileFinder
#include "obse/GameMenus.h"
#include "Miscellaneous/InGameDate.h"

XXNHUDClockMenu* XXNHUDClockMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNHUDClockMenu));
   if (memory) {
      auto result = new (memory) XXNHUDClockMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNHUDClockMenu::XXNHUDClockMenu() {
};
XXNHUDClockMenu::~XXNHUDClockMenu() {
};

void XXNHUDClockMenu::Dispose(bool free) {
   this->~XXNHUDClockMenu();
   if (free) {
      FormHeap_Free(this);
   }
};
void XXNHUDClockMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
};
UInt32 XXNHUDClockMenu::GetID() {
   return kID;
};
void XXNHUDClockMenu::HandleFrame() {
   this->Synch();
   this->Render();
};

void XXNHUDClockMenu::Render() {
   auto root = (RE::Tile*) this->tile;
   CALL_MEMBER_FN(root, UpdateFloat)(kRootTrait_RawGameHour, this->date.rawGameHour);
   {
      UInt32 i = 0;
      do {
         const char* format = CALL_MEMBER_FN(root, GetStringTraitValue)(kRootTrait_FormatString1 + i);
         char formatted[256];
         this->date.Format(formatted, format);
         CALL_MEMBER_FN(root, UpdateString)(kRootTrait_FormattedOutput1 + i, formatted);
      } while (++i < kRoot_NumberOfFormattedDates);
   }
};
void XXNHUDClockMenu::Synch() {
   this->date.SetFromTime(RE::g_timeGlobals);
};

// ------------------------

RE::Tile* ShowXXNHUDClockMenu() {
   if ((*g_FileFinder)->FindFile("Data\\Menus\\NorthernUI\\xxnhudclockmenu.xml", 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
      _MESSAGE("ShowXXNHUDClockMenu: The XML file for the XXNHUDClockMenu is missing. The menu will not be opened.");
      return nullptr;
   }
   if (auto existing = GetMenuByType(XXNHUDClockMenu::kID)) {
      ((XXNHUDClockMenu*)existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)("Data\\Menus\\NorthernUI\\xxnhudclockmenu.xml");
   auto menu = (XXNHUDClockMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu)
      return nullptr;
   if (menu->GetID() != XXNHUDClockMenu::kID) {
      menu->Dispose(true);
      return nullptr;
   }
   {  // Vital menu setup tasks:
      //
      // These tasks SHOULD be performed for us by TileMenu::UpdateField, but they aren't, and it seems 
      // like every ShowWhateverMenu subroutine does them manually anyway. If they're not performed, 
      // then a wide variety of major bugs will occur, including:
      //
      //  - The menu cannot be added to InterfaceManager::activeMenuIDs, and consequently cannot receive 
      //    keyboard input events or affect other menus (with respect to modals)
      //
      //  - The menu will not close properly if the user pauses and loads a savegame while it's open
      //
      CALL_MEMBER_FN(menu, RegisterTile)(tile);
      {  // set menu depth
         float stackingtype = CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_stackingtype);
         if (stackingtype == 6006.0F || stackingtype == 102.0F) {
            float depth = RE::GetNewMenuDepth();
            CALL_MEMBER_FN(tile, UpdateFloat)(kTileValue_depth, depth);
         }
      }
   }
   CALL_MEMBER_FN(menu, EnableMenu)(0);
   return tile;
};