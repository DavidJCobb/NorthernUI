#include "XXNHUDInputViewerMenu.h"
#include "Patches/XboxGamepad/Main.h"
#include "ReverseEngineered/Miscellaneous.h"
#include "ReverseEngineered/UI/InterfaceManager.h"
#include "obse/GameAPI.h"
#include "obse/GameData.h" // FileFinder
#include "obse/GameMenus.h"

XXNHUDInputViewerMenu* XXNHUDInputViewerMenu::Create() {
   void* memory = FormHeap_Allocate(sizeof(XXNHUDInputViewerMenu));
   if (memory) {
      auto result = new (memory) XXNHUDInputViewerMenu(); // "placement new:" construct an object at already-allocated memory: new (pointer) Class(args);
      return result;
   }
   return nullptr;
};
XXNHUDInputViewerMenu::XXNHUDInputViewerMenu() {
};
XXNHUDInputViewerMenu::~XXNHUDInputViewerMenu() {
};

void XXNHUDInputViewerMenu::Dispose(bool free) {
   this->~XXNHUDInputViewerMenu();
   if (free) {
      FormHeap_Free(this);
   }
};
void XXNHUDInputViewerMenu::HandleTileIDChange(SInt32 newID, RE::Tile* tile) {
};
UInt32 XXNHUDInputViewerMenu::GetID() {
   return kID;
};
void XXNHUDInputViewerMenu::HandleFrame() {
   auto tile = this->tile;
   {  // visibility check
      if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_visible) == 1.0F)
         return;
      if (CALL_MEMBER_FN(tile, GetFloatTraitValue)(kTileValue_listclip) == 2.0F)
         return;
   }
   XXNGamepad* gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
   if (!gamepad) {
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_CurrentGamepadIndex, -1.0F);
      return;
   }
   CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_CurrentGamepadIndex, gamepad->index);
   {  // Bitmasks
      SInt32 mask;
      {  // ABXY
         mask = 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_A, RE::kKeyQuery_Hold) << 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_B, RE::kKeyQuery_Hold) << 1;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_X, RE::kKeyQuery_Hold) << 2;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_Y, RE::kKeyQuery_Hold) << 3;
         CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputBitmaskABXY, mask);
      }
      {  // D-Pad
         mask = 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadUp,    RE::kKeyQuery_Hold) << 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadDown,  RE::kKeyQuery_Hold) << 1;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadLeft,  RE::kKeyQuery_Hold) << 2;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_DPadRight, RE::kKeyQuery_Hold) << 3;
         CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputBitmaskDPad, mask);
      }
      {  // Sticks and triggers
         mask = 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_LS, RE::kKeyQuery_Hold) << 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_RS, RE::kKeyQuery_Hold) << 1;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_SpecialCaseLT, RE::kKeyQuery_Hold) << 2;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_SpecialCaseRT, RE::kKeyQuery_Hold) << 3;
         CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputBitmaskStickTrig, mask);
      }
      {  // Bumpers and system
         mask = 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_LB,    RE::kKeyQuery_Hold) << 0;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_RB,    RE::kKeyQuery_Hold) << 1;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_Back,  RE::kKeyQuery_Hold) << 2;
         mask |= gamepad->GetButtonState(XXNGamepad::kGamepadButton_Start, RE::kKeyQuery_Hold) << 3;
         CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputBitmaskBumpSys, mask);
      }
   }
   {  // Joystick inputs
      SInt32 Lx = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_LeftX);
      SInt32 Ly = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_LeftY);
      SInt32 Rx = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_RightX);
      SInt32 Ry = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_RightY);
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputLeftStickX,  Lx);
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputLeftStickY,  Ly);
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputRightStickX, Rx);
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputRightStickY, Ry);
      //
      float distance = (Lx*Lx) + (Ly*Ly);
      distance = sqrt(distance);
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputLeftStickMag, distance);
      //
      distance = (Rx*Rx) + (Ry*Ry);
      distance = sqrt(distance);
      CALL_MEMBER_FN(tile, UpdateFloat)(kRootTrait_InputRightStickMag, distance);
   }
};

// ------------------------

RE::Tile* ShowXXNHUDInputViewerMenu() {
   if ((*g_FileFinder)->FindFile(XXNHUDInputViewerMenu::menuPath, 0, 0, -1) == FileFinder::kFileStatus_NotFound) {
      _MESSAGE("ShowXXNHUDInputViewerMenu: The XML file for the XXNHUDInputViewerMenu is missing. The menu will not be opened.");
      return nullptr;
   }
   if (auto existing = GetMenuByType(XXNHUDInputViewerMenu::kID)) {
      ((XXNHUDInputViewerMenu*)existing)->Dispose(true);
   }
   auto ui = RE::InterfaceManager::GetInstance();
   auto tile = CALL_MEMBER_FN(ui->menuRoot, ReadXML)(XXNHUDInputViewerMenu::menuPath);
   auto menu = (XXNHUDInputViewerMenu*) CALL_MEMBER_FN(tile, GetContainingMenu)();
   if (!menu)
      return nullptr;
   if (menu->GetID() != XXNHUDInputViewerMenu::kID) {
      _MESSAGE("ShowXXNHUDInputViewerMenu: Failed to create the correct menu. Retrieved menu ID is %d.", menu->GetID());
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