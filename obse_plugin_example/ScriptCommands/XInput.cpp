#include "XInput.h"
#include "obse/GameAPI.h"
#include "obse/ParamInfos.h"
#include "Patches/XboxGamepad/Main.h"
#include "ReverseEngineered/Systems/Input.h"

//
// TODO:
//
//  - Allow the script author to specify which gamepad these commands should affect. 
//    For "read" commands, -1 should mean "the first connected gamepad," while for 
//    "write" commands, -1 should mean "all gamepads" and should be what's recommended.
//
//  - Add a command for checking whether a given gamepad is connected.
//
//  - Add a command for getting the number of the first connected gamepad.
//
//  - Add commands for getting the joystick coordinates and exact trigger magnitudes.
//
//     - For joysticks, follow the example set by GetCursorPos for letting the 
//       script author pick which axis to return. For simplicity's sake we could 
//       also allow scripts to request the joystick magnitude instead of an axis.
//
//  - Add a command for checking various aspects of the player's input settings, 
//    such as which joystick is mapped to menu input, the current "run sensitivity," 
//    the current look sensitivity, and so on.
//
//  - OBSE OnKeyDown is described like an event registration but seems to just be a 
//    simple getter for if a key went down on the current frame (although perhaps 
//    there is some registration-like stuff under the hood to allow for scripts that 
//    can't reasonably run on every frame); if it's a simple getter or would be easy 
//    to recreate, then recreate it; ditto for OnControlDown and maybe some key-up 
//    stuff as well
//
//  - IsControlPressed
//

namespace Exec {
   static bool IsGamepadKeyPressed(COMMAND_ARGS) { // Analogous to IsKeyPressed and IsKeyPressed2
      *result = 0.0;
      UInt32 keycode = 0;
      if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode))
         return true;
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
      if (gamepad && gamepad->GetButtonState(keycode, RE::kKeyQuery_Hold, false))
         *result = 1.0;
      return true;
   }
   static bool IsGamepadKeyPressedEvenIfDisabled(COMMAND_ARGS) { // Analogous to IsKeyPressed3
      *result = 0.0;
      UInt32 keycode = 0;
      if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode))
         return true;
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
      if (gamepad && gamepad->GetButtonState(keycode, RE::kKeyQuery_Hold, true))
         *result = 1.0;
      return true;
   }
   static bool DisableGamepadKey(COMMAND_ARGS) {
      *result = 0.0;
      UInt32 keycode = 0;
      if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode))
         return true;
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
      if (gamepad)
         gamepad->SetDisableState(keycode, true);
      return true;
   }
   static bool EnableGamepadKey(COMMAND_ARGS) {
      *result = 0.0;
      UInt32 keycode = 0;
      if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode))
         return true;
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
      if (gamepad)
         gamepad->SetDisableState(keycode, false);
      return true;
   }
   static bool IsGamepadKeyDisabled(COMMAND_ARGS) {
      *result = 0.0;
      UInt32 keycode = 0;
      if (ExtractArgs(PASS_EXTRACT_ARGS, &keycode)) {
         auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(0);
         if (gamepad && gamepad->GetDisableState(keycode))
            *result = 1.0;
      }
      return true;
   }

   // Get the gamepad button to which a given control is mapped.
   static bool GetGamepadControl(COMMAND_ARGS) {
      *result = -1.0;
      UInt32 control = 0;
      if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &control))
         return true;
      if (control > RE::kControl_COUNT)
         return true;
      auto input = RE::OSInputGlobals::GetInstance();
      if (input)
         *result = input->joystick[control];
      return true;
   }
}

CommandInfo kCommandInfo_IsGamepadKeyPressed = {
   "XXNIsGamepadKeyPressed",
   "xxn_igkp",
   0,
   "Returns true if the gamepad button is pressed.",
   0,
   1,
   kParams_OneInt,
   HANDLER(Exec::IsGamepadKeyPressed),
   Cmd_Default_Parse,
   nullptr,
   0
};
CommandInfo kCommandInfo_IsGamepadKeyPressedEvenIfDisabled = {
   "XXNIsGamepadKeyPressedEvenIfDisabled",
   "xxn_igkp_honest",
   0,
   "Returns true if the gamepad button is pressed, even if a script has disabled that button.",
   0,
   1,
   kParams_OneInt,
   HANDLER(Exec::IsGamepadKeyPressedEvenIfDisabled),
   Cmd_Default_Parse,
   nullptr,
   0
};
CommandInfo kCommandInfo_DisableGamepadKey = {
   "XXNDisableGamepadKey",
   "xxn_dgk",
   0,
   "Prevents the game from reacting to a gamepad button.",
   0,
   1,
   kParams_OneInt,
   HANDLER(Exec::DisableGamepadKey),
   Cmd_Default_Parse,
   nullptr,
   0
};
CommandInfo kCommandInfo_EnableGamepadKey = {
   "XXNEnableGamepadKey",
   "xxn_egk",
   0,
   "Re-enables a gamepad button previously disabled with DisableGamepadKey.",
   0,
   1,
   kParams_OneInt,
   HANDLER(Exec::EnableGamepadKey),
   Cmd_Default_Parse,
   nullptr,
   0
};
CommandInfo kCommandInfo_IsGamepadKeyDisabled = {
   "XXNIsGamepadKeyDisabled",
   "xxn_igkd",
   0,
   "Returns 1 if the gamepad button is disabled, or 0 otherwise.",
   0,
   1,
   kParams_OneInt,
   HANDLER(Exec::IsGamepadKeyDisabled),
   Cmd_Default_Parse,
   nullptr,
   0
};
CommandInfo kCommandInfo_GetGamepadControl = {
   "XXNGetGamepadControl",
   "xxn_ggc",
   0,
   "Return the gamepad button index that a given control has been assigned to.",
   0,
   1,
   kParams_OneInt,
   HANDLER(Exec::GetGamepadControl),
   Cmd_Default_Parse,
   nullptr,
   0
};