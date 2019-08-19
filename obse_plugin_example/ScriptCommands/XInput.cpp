#include "XInput.h"
#include "obse/GameAPI.h"
#include "obse/ParamInfos.h"
#include "Patches/XboxGamepad/Main.h"
#include "ReverseEngineered/Systems/Input.h"
#include "obse/PluginAPI.h"

//
// TODO:
//
//  - Add a command for getting the number of the first connected gamepad.
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

constexpr SInt32 ce_anyGamepad   = -1; // for "read"  commands, gamepad index -1 means "any gamepad"
constexpr SInt32 ce_everyGamepad = -1; // for "write" commands, gamepad index -1 means "every gamepad"

constexpr UInt32 ce_opcode_auto   = 0;
constexpr UInt32 ce_is_static     = 0;
constexpr UInt32 ce_flags_default = 0;

ParamInfo kParams_Gamepad[1] = {
   { "int gamepad index",  kParamType_Integer, 0 },
};
ParamInfo kParams_Gamepad_Key_Flags[3] = {
   { "int gamepad index",  kParamType_Integer, 0 },
   { "int gamepad button", kParamType_Integer, 0 },
   { "int options flags",  kParamType_Integer, 0 },
};
ParamInfo kParams_GamepadAndKey[2] = {
   { "int gamepad index",  kParamType_Integer, 0 },
   { "int gamepad button", kParamType_Integer, 0 },
};
ParamInfo kParams_Control[1] = {
   { "int control index",  kParamType_Integer, 0 },
};
ParamInfo kParams_JoystickState[4] = {
   { "int gamepad index",  kParamType_Integer, 0 },
   { "int which joystick", kParamType_Integer, 0 },
   { "int which axis",     kParamType_Integer, 0 },
   { "int options flags",  kParamType_Integer, 0 },
};
ParamInfo kParams_TriggerState[3] = {
   { "int gamepad index", kParamType_Integer, 0 },
   { "int which trigger", kParamType_Integer, 0 },
   { "int options flags", kParamType_Integer, 0 },
};

#define XXN_COMMAND(n, sn, d, args, exec) { n, sn, ce_opcode_auto, d, ce_is_static, std::extent<decltype(args)>::value, args, exec, nullptr, nullptr, ce_flags_default };

#define XXN_CMD_EXPLANATION_ANY_GAMEPAD  "Gamepad index -1 means \"any gamepad.\""
#define XXN_CMD_EXPLANATION_ALL_GAMEPADS "Gamepad index -1 means \"all gamepads.\""
#define XXN_CMD_EXPLANATION_CHECK_FLAGS  "If the flags argument has bit 1 set, then we ignore scripted attempts at disabling keys and return the key's true state."

static bool Cmd_Exec_IsGamepadKeyPressed(COMMAND_ARGS) { // Analogous to IsKeyPressed and IsKeyPressed2
   *result = 0.0;
   SInt32 gpIndex = ce_anyGamepad;
   UInt32 keycode = 0;
   bool   honest;
   {
      UInt32 flags = 0;
      if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex, &keycode, &flags))
         return true;
      bool honest = flags & 1;
   }
   if (gpIndex != ce_anyGamepad) {
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
      if (gamepad && gamepad->GetButtonState(keycode, RE::kKeyQuery_Hold, honest))
         *result = 1.0;
   } else {
      auto core = XXNGamepadSupportCore::GetInstance();
      gpIndex = 0;
      do {
         auto gamepad = core->GetGamepad(gpIndex);
         if (gamepad && gamepad->GetButtonState(keycode, RE::kKeyQuery_Hold, honest)) {
            *result = 1.0;
            break;
         }
      } while (++gpIndex < 4);
   }
   return true;
}
CommandInfo kCommandInfo_IsGamepadKeyPressed = XXN_COMMAND(
   "XXNIsGamepadKeyPressed",
   "xxn_igkp",
   "Returns true if the gamepad button is pressed. " XXN_CMD_EXPLANATION_ANY_GAMEPAD " " XXN_CMD_EXPLANATION_CHECK_FLAGS,
   kParams_Gamepad_Key_Flags,
   Cmd_Exec_IsGamepadKeyPressed
);

static bool Cmd_Exec_DisableGamepadKey(COMMAND_ARGS) {
   *result = 0.0;
   SInt32 gpIndex = ce_everyGamepad;
   UInt32 keycode = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex, &keycode))
      return true;
   if (gpIndex == ce_everyGamepad) {
      auto core = XXNGamepadSupportCore::GetInstance();
      gpIndex = 0;
      do {
         auto gamepad = core->GetGamepad(gpIndex);
         if (gamepad)
            gamepad->SetDisableState(keycode, true);
      } while (++gpIndex < 4);
   } else {
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
      if (gamepad)
         gamepad->SetDisableState(keycode, true);
   }
   return true;
}
CommandInfo kCommandInfo_DisableGamepadKey = XXN_COMMAND(
   "XXNDisableGamepadKey",
   "xxn_dgk",
   "Prevents the game from reacting to a gamepad button. " XXN_CMD_EXPLANATION_ALL_GAMEPADS,
   kParams_GamepadAndKey,
   Cmd_Exec_DisableGamepadKey
);

static bool Cmd_Exec_EnableGamepadKey(COMMAND_ARGS) {
   *result = 0.0;
   SInt32 gpIndex = ce_everyGamepad;
   UInt32 keycode = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex, &keycode))
      return true;
   if (gpIndex == ce_everyGamepad) {
      auto core = XXNGamepadSupportCore::GetInstance();
      gpIndex = 0;
      do {
         auto gamepad = core->GetGamepad(gpIndex);
         if (gamepad)
            gamepad->SetDisableState(keycode, false);
      } while (++gpIndex < 4);
   } else {
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
      if (gamepad)
         gamepad->SetDisableState(keycode, false);
   }
   return true;
}
CommandInfo kCommandInfo_EnableGamepadKey = XXN_COMMAND(
   "XXNEnableGamepadKey",
   "xxn_egk",
   "Re-enables a gamepad button previously disabled with DisableGamepadKey. " XXN_CMD_EXPLANATION_ALL_GAMEPADS,
   kParams_GamepadAndKey,
   Cmd_Exec_EnableGamepadKey
);

static bool Cmd_Exec_IsGamepadKeyDisabled(COMMAND_ARGS) {
   *result = 0.0;
   SInt32 gpIndex = ce_anyGamepad;
   UInt32 keycode = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex, &keycode))
      return true;
   if (gpIndex == ce_anyGamepad) {
      auto core = XXNGamepadSupportCore::GetInstance();
      gpIndex = 0;
      do {
         auto gamepad = core->GetGamepad(gpIndex);
         if (gamepad && gamepad->GetDisableState(keycode)) {
            *result = 1.0;
            break;
         }
      } while (++gpIndex < 4);
   } else {
      auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
      if (gamepad && gamepad->GetDisableState(keycode))
         *result = 1.0;
   }
   return true;
}
CommandInfo kCommandInfo_IsGamepadKeyDisabled = XXN_COMMAND(
   "XXNIsGamepadKeyDisabled",
   "xxn_igkd",
   "Returns 1 if the gamepad button is disabled, or 0 otherwise. " XXN_CMD_EXPLANATION_ANY_GAMEPAD,
   kParams_GamepadAndKey,
   Cmd_Exec_IsGamepadKeyDisabled
);

static bool Cmd_Exec_GetGamepadControl(COMMAND_ARGS) {
   *result = -1.0;
   UInt32 control = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &control))
      return true;
   if (control > RE::kControl_COUNT)
      return true;
   auto input = RE::OSInputGlobals::GetInstance();
   if (input)
      *result = input->joystick[control];
   return true;
}
CommandInfo kCommandInfo_GetGamepadControl = XXN_COMMAND(
   "XXNGetGamepadControl",
   "xxn_ggc",
   "Return the gamepad button index that a given control has been assigned to.",
   kParams_Control,
   Cmd_Exec_GetGamepadControl
);

static bool Cmd_Exec_IsGamepadConnected(COMMAND_ARGS) {
   *result = 0.0;
   UInt32 gpIndex = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex))
      return true;
   auto gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
   if (gamepad && gamepad->isConnected)
      *result = 1.0;
   return true;
}
CommandInfo kCommandInfo_IsGamepadConnected = XXN_COMMAND(
   "XXNIsGamepadConnected",
   "xxn_igc",
   "Returns 1 if the specified gamepad is connected, or 0 otherwise.",
   kParams_Gamepad,
   Cmd_Exec_IsGamepadConnected
);

static bool Cmd_Exec_GetGamepadJoystickMagnitude(COMMAND_ARGS) {
   enum Stick {
      kStick_Left  = 0,
      kStick_Right = 0,
   };
   enum Axis {
      kAxis_Magnitude = 0,
      kAxis_X = 1,
      kAxis_Y = 2,
   };
   //
   *result = 0.0;
   UInt32 gpIndex = 0;
   UInt32 stick   = 0;
   UInt32 axis    = 0;
   UInt32 flags   = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex, &stick, &axis, &flags))
      return true;
   XXNGamepad* gamepad;
   if (gpIndex == -1)
      gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
   else
      gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
   if (!gamepad || !gamepad->isConnected)
      return true;
   float x;
   float y;
   if (stick == kStick_Left) {
      x = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_LeftX);
      y = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_LeftY);
   } else if (stick == kStick_Right) {
      x = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_RightX);
      y = gamepad->GetJoystickAxis(XXNGamepad::kJoystickAxis_RightY);
   } else
      return true;
   switch (axis) {
      case kAxis_Magnitude:
         *result = sqrt((x * x) + (y * y));
         break;
      case kAxis_X:
         *result = x;
         break;
      case kAxis_Y:
         *result = y;
         break;
   }
   return true;
}
CommandInfo kCommandInfo_GetGamepadJoystickMagnitude = XXN_COMMAND(
   "XXNGetGamepadJoystickMagnitude",
   "xxn_ggjm",
   "Returns the magnitude of the specified joystick along the specified axis. " XXN_CMD_EXPLANATION_ANY_GAMEPAD " Allowed joysticks are left (0) or right (1); allowed axes are total magnitude (0), x (1), or y (2).",
   kParams_JoystickState,
   Cmd_Exec_GetGamepadJoystickMagnitude
);

static bool Cmd_Exec_GetGamepadTriggerMagnitude(COMMAND_ARGS) {
   enum Trigger {
      kTrigger_Left = 0,
      kTrigger_Right = 0,
   };
   enum OptionFlags {
      kOptionFlags_Honest = 1,
   };
   //
   *result = 0.0;
   UInt32 gpIndex = 0;
   UInt32 trigger = 0;
   UInt32 flags   = 0;
   if (!ExtractArgs(PASS_EXTRACT_ARGS, &gpIndex, &trigger, &flags))
      return true;
   bool honest = (flags & kOptionFlags_Honest);
   XXNGamepad* gamepad;
   if (gpIndex == -1)
      gamepad = XXNGamepadSupportCore::GetInstance()->GetAnyGamepad();
   else
      gamepad = XXNGamepadSupportCore::GetInstance()->GetGamepad(gpIndex);
   if (!gamepad || !gamepad->isConnected)
      return true;
   if (trigger == kTrigger_Left) {
      if (honest || !gamepad->disabled.bLeftTrigger)
         *result = gamepad->thisFrame.bLeftTrigger;
   } else if (trigger == kTrigger_Right) {
      if (honest || !gamepad->disabled.bRightTrigger)
         *result = gamepad->thisFrame.bRightTrigger;
   }
   return true;
}
CommandInfo kCommandInfo_GetGamepadTriggerMagnitude = XXN_COMMAND(
   "XXNGetGamepadTriggerMagnitude",
   "xxn_ggtm",
   "Returns the magnitude of the specified trigger. " XXN_CMD_EXPLANATION_ANY_GAMEPAD " Allowed triggers are left (0) or right (1). " XXN_CMD_EXPLANATION_CHECK_FLAGS,
   kParams_TriggerState,
   Cmd_Exec_GetGamepadTriggerMagnitude
);