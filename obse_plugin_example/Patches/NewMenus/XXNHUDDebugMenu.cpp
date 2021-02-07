#include "XXNHUDDebugMenu.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"

#pragma region XXNHUDDebugMenu::XInput
bool XXNHUDDebugMenu::XInput::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
   if (newID == kTileID_SmoothWASDJoystick) {
      this->smoothWASDJoystick = tile;
      return true;
   } else if (newID == kTileID_SmoothWASDStatus) {
      this->smoothWASDStatus = tile;
      return true;
   }
   return false;
}
void XXNHUDDebugMenu::XInput::ShowSmoothWASDJoystick(SInt32 x, SInt32 y, float angle, SInt8 angleSlice, bool isPerfectDirection) {
   auto& tile = this->smoothWASDJoystick;
   if (!tile)
      return;
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDJoystickTrait_Angle, angle);
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDJoystickTrait_AngleSlice, angleSlice);
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDJoystickTrait_X, x);
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDJoystickTrait_Y, y);
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDJoystickTrait_IsPerfect8Direction, isPerfectDirection ? 2.0F : 1.0F);
}
void XXNHUDDebugMenu::XInput::ShowSmoothWASDStatus(SInt8 dominant, SInt8 ancillary, bool isPressingAncillary) {
   auto& tile = this->smoothWASDStatus;
   if (!tile)
      return;
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDStatusTrait_DirectionDominant,  dominant);
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDStatusTrait_DirectionAncillary, ancillary);
   CALL_MEMBER_FN(tile, UpdateFloat)(kSmoothWASDStatusTrait_IsPressingAncillary, isPressingAncillary ? 2.0F : 1.0F);
}
#pragma endregion
#pragma region XXNHUDDebugMenu::EnhancedMovement
bool XXNHUDDebugMenu::EnhancedMovement::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
   if (newID == tile_id) {
      this->root = tile;
      return true;
   }
   return false;
}
#pragma endregion

// ---------------------------------------------------------------------------------------------------------------------------------------------

XXNHUDDebugMenu* XXNHUDDebugMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNHUDDebugMenu));
   if (memory) {
      auto result = new (memory) XXNHUDDebugMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNHUDDebugMenu* XXNHUDDebugMenu::Get(bool createIfNeeded) {
   if (kID - kMenuType_Message >= g_TileMenuArray->capacity) { // make sure the expanded IDs patch has been applied
      return nullptr;
   }
   auto tile = g_TileMenuArray->data[kID - kMenuType_Message];
   if (!tile) {
      if (!createIfNeeded)
         return nullptr;
      tile = (TileMenu*) ShowXXNHUDDebugMenu();
      if (!tile)
         return nullptr;
   }
   return (XXNHUDDebugMenu*) tile->menu;
};
XXNHUDDebugMenu::XXNHUDDebugMenu() {
};
XXNHUDDebugMenu::~XXNHUDDebugMenu() {
};

void XXNHUDDebugMenu::Dispose(bool free) {
   this->~XXNHUDDebugMenu();
   if (free) {
      FormHeap_Free(this);
   }
};
void XXNHUDDebugMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
   if (this->handlerXInput.HandleTileIDChange(newID, tile))
      return;
   if (this->handlerEnhMovement.HandleTileIDChange(newID, tile))
      return;
};
UInt32 XXNHUDDebugMenu::GetID() {
   return kID;
};

RE::Tile* ShowXXNHUDDebugMenu() {
   if (auto existing = GetMenuByType(XXNHUDDebugMenu::kID)) {
      ((XXNHUDDebugMenu*) existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)("Data\\Menus\\northernui\\xxnhuddebugmenu.xml");
   auto menu = (XXNHUDDebugMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu)
      return nullptr;
   if (menu->GetID() != XXNHUDDebugMenu::kID) {
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