#include "Main.h"
#include "ReverseEngineered/Systems/Input.h"

HMODULE g_xInputDllHandle = 0;
bool g_xInputPatchApplied = false;

constexpr SInt32 OBLIVION_JOYSTICK_MIN = -100;
constexpr SInt32 OBLIVION_JOYSTICK_MAX = 100;

__declspec(noinline) XXNGamepadSupportCore* XXNGamepadSupportCore::GetInstance() {
   static XXNGamepadSupportCore _core;
   return &_core;
};

/*static*/ const char* XXNGamepad::GetButtonName(UInt8 button) {
   switch (button) {
      case kGamepadButton_A:
         return "A";
      case kGamepadButton_B:
         return "B";
      case kGamepadButton_X:
         return "X";
      case kGamepadButton_Y:
         return "Y";
      case kGamepadButton_LB:
         return "LB";
      case kGamepadButton_RB:
         return "RB";
      case kGamepadButton_LS:
         return "LS";
      case kGamepadButton_RS:
         return "RS";
      case kGamepadButton_SpecialCaseLT:
         return "LT";
      case kGamepadButton_SpecialCaseRT:
         return "RT";
      case kGamepadButton_Back:
         return "Back";
      case kGamepadButton_Start:
         return "Start";
      case kGamepadButton_DPadUp:
         return "D-Pad Up";
      case kGamepadButton_DPadDown:
         return "D-Pad Down";
      case kGamepadButton_DPadLeft:
         return "D-Pad Left";
      case kGamepadButton_DPadRight:
         return "D-Pad Right";
   }
   return nullptr;
};
/*static*/ UInt32 XXNGamepad::GetButtonCode(const char* name) {
   if (!name)
      return 0xFF;
   if (_stricmp(name, "A") == 0)
      return kGamepadButton_A;
   if (_stricmp(name, "B") == 0)
      return kGamepadButton_B;
   if (_stricmp(name, "X") == 0)
      return kGamepadButton_X;
   if (_stricmp(name, "Y") == 0)
      return kGamepadButton_Y;
   if (_stricmp(name, "LB") == 0)
      return kGamepadButton_LB;
   if (_stricmp(name, "RB") == 0)
      return kGamepadButton_RB;
   if (_stricmp(name, "LS") == 0)
      return kGamepadButton_LS;
   if (_stricmp(name, "RS") == 0)
      return kGamepadButton_RS;
   if (_stricmp(name, "LT") == 0)
      return kGamepadButton_SpecialCaseLT;
   if (_stricmp(name, "RT") == 0)
      return kGamepadButton_SpecialCaseRT;
   if (_stricmp(name, "Back") == 0)
      return kGamepadButton_Back;
   if (_stricmp(name, "Start") == 0)
      return kGamepadButton_Start;
   if (_stricmp(name, "D-Pad Up") == 0)
      return kGamepadButton_DPadUp;
   if (_stricmp(name, "D-Pad Down") == 0)
      return kGamepadButton_DPadDown;
   if (_stricmp(name, "D-Pad Left") == 0)
      return kGamepadButton_DPadLeft;
   if (_stricmp(name, "D-Pad Right") == 0)
      return kGamepadButton_DPadRight;
   return 0xFF;
}

UInt32 XInputControlBindingToDXScanCode(const UInt32 binding) {
   if (binding == 0xFF)
      return 0;
   if (binding >= XXNGamepad::kGamepadButton_Unused0400)
      return binding + 0x10A - 2;
   return binding + 0x10A;
};

XXNGamepad::XXNGamepad() {
   ZeroMemory(&this->lastFrame, sizeof(this->lastFrame));
   ZeroMemory(&this->thisFrame, sizeof(this->thisFrame));
};

void XXNGamepad::Update() {
   memcpy(&this->lastFrame, &this->thisFrame, sizeof(this->thisFrame));
   XINPUT_STATE state;
   memset(&state, 0, sizeof(state));
   DWORD result = XInputGetState(this->index, &state);
   if (result == ERROR_SUCCESS) { // "Error: Success!" lol
      memcpy(&this->thisFrame, &state.Gamepad, sizeof(this->thisFrame));
      this->isConnected = true;
   } else {
      //
      // result == error code, defined in winerror.h; relevant values include and are not limited to:
      //
      // 1164 == The indicated device requires reinitialization due to hardware errors.
      // 1167 == The device is not connected.
      //
      memset(&this->thisFrame, 0, sizeof(this->thisFrame));
      if (result == 1167 && this->isConnected) { // device disconnected
         memset(&this->lastFrame, 0, sizeof(this->lastFrame));
         this->isConnected = false;
      }
   }
};
SInt32 XXNGamepad::AnyKeyMatches(KeyQuery state) const {
   UInt32 i = kGamepadButton_DPadUp;
   do {
      if (this->GetButtonState(i, state))
         return i;
   } while (++i < kGamepadButton_Unused0400);
   i = kGamepadButton_A;
   do {
      if (this->GetButtonState(i, state))
         return i;
   } while (++i < kGamepadButton_COUNT);
   return -1;
};
void XXNGamepad::Clear() {
   ZeroMemory(&this->lastFrame, sizeof(this->lastFrame));
   ZeroMemory(&this->thisFrame, sizeof(this->thisFrame));
};
bool XXNGamepad::GetButtonState(UInt8 button, KeyQuery state) const {
   if (button > kGamepadButton_LastStandardButton) { // special cases
      bool now;
      bool before;
      switch (button) {
         case kGamepadButton_SpecialCaseLT:
            now    = this->thisFrame.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            before = this->lastFrame.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            break;
         case kGamepadButton_SpecialCaseRT:
            now    = this->thisFrame.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            before = this->lastFrame.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            break;
         default:
            return false;
      }
      if (state == KeyQuery::kKeyQuery_Hold) // key hold
         return now;
      if (state == KeyQuery::kKeyQuery_Down) // key down
         return now && !before;
      if (state == KeyQuery::kKeyQuery_Up) // key up
         return before && !now;
      if (state == KeyQuery::kKeyQuery_Change) // key change
         return now != before;
      return false;
   }
   bool now = (this->thisFrame.wButtons >> button) & 1;
   if (state == KeyQuery::kKeyQuery_Hold) // key hold
      return now;
   bool before = (this->lastFrame.wButtons >> button) & 1;
   if (state == KeyQuery::kKeyQuery_Down) // key down
      return now && !before;
   if (state == KeyQuery::kKeyQuery_Up)
      return !now && before;
   if (state == KeyQuery::kKeyQuery_Change)
      return now != before;
   return false;
};
SInt32 XXNGamepad::GetJoystickAxis(UInt8 axis) const {
   SInt32 v;
   SInt32 deadzone; // vanilla Oblivion uses a 10% deadzone, but we'll use XInput's deadzones for now
   switch (axis) {
      default:
      case kJoystickAxis_LeftZ:
      case kJoystickAxis_RightZ:
         return 0;
      case kJoystickAxis_LeftX:
         v = this->thisFrame.sThumbLX;
         deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
         break;
      case kJoystickAxis_LeftY:
         v = -this->thisFrame.sThumbLY; // I guess XInput and DInput have opposing vertical axes?
         deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
         break;
      case kJoystickAxis_RightX:
         v = this->thisFrame.sThumbRX;
         deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
         break;
      case kJoystickAxis_RightY:
         v = -this->thisFrame.sThumbRY; // I guess XInput and DInput have opposing vertical axes?
         deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
         break;
   }
   if (v >= 0) {
      if (v < deadzone)
         return 0;
      v -= deadzone;
   } else {
      v += deadzone; // === v -= -deadzone;
      if (v > 0)
         return 0;
   }
   float normalized = ((float)v) / (32767 - deadzone) * OBLIVION_JOYSTICK_MAX;
   return (SInt32) normalized;
};
void XXNGamepad::SendControlPress(UInt8 button) {
   if (button >= 0x80) { // special cases
      bool now;
      bool before;
      switch (button) {
         case kGamepadButton_SpecialCaseLT:
            this->thisFrame.bLeftTrigger = XINPUT_GAMEPAD_TRIGGER_THRESHOLD + 1;
            break;
         case kGamepadButton_SpecialCaseRT:
            this->thisFrame.bRightTrigger = XINPUT_GAMEPAD_TRIGGER_THRESHOLD + 1;
            break;
      }
      return;
   }
   this->thisFrame.wButtons |= 1 << button;
}
void XXNGamepad::SetVibrationSpeeds(UInt16 speed) const {
   XINPUT_VIBRATION vibration;
   vibration.wLeftMotorSpeed  = speed; // use any value between 0-65535 here
   vibration.wRightMotorSpeed = speed; // use any value between 0-65535 here
   XInputSetState(this->index, &vibration);
};
void XXNGamepad::SetVibrationSpeeds(UInt16 speedL, UInt16 speedR) const {
   XINPUT_VIBRATION vibration;
   vibration.wLeftMotorSpeed  = speedL; // use any value between 0-65535 here
   vibration.wRightMotorSpeed = speedR; // use any value between 0-65535 here
   XInputSetState(this->index, &vibration);
};

// ----------------------------------------------------------------------

XXNGamepadSupportCore::XXNGamepadSupportCore() {
   UInt32 i = 0;
   do {
      this->gamepads[i].index = i;
   } while (++i < std::extent<decltype(this->gamepads)>::value);
};
void XXNGamepadSupportCore::Disable() {
   this->disabled = true;
   this->anyConnected = false;
   UInt32 i = 0;
   do {
      this->gamepads[i].Clear();
      this->gamepads[i].isConnected = false;
   } while (++i < std::extent<decltype(this->gamepads)>::value);
};
XXNGamepad* XXNGamepadSupportCore::GetAnyGamepad() {
   UInt8 i = 0;
   do {
      auto gamepad = &(this->gamepads[i]);
      if (gamepad->isConnected)
         return gamepad;
   } while (++i < std::extent<decltype(this->gamepads)>::value);
   return nullptr;
};
UInt8 XXNGamepadSupportCore::GetBoundKey(UInt8 control) const {
   return RE::OSInputGlobals::GetInstance()->joystick[control];
};
XXNGamepad* XXNGamepadSupportCore::GetGamepad(UInt8 index) {
   if (index >= std::extent<decltype(this->gamepads)>::value)
      return nullptr;
   return &this->gamepads[index];
};
void XXNGamepadSupportCore::Update() {
   if (this->disabled)
      return;
   this->anyConnected = false;
   UInt8 i = 0;
   do {
      auto& device = this->gamepads[i];
      device.Update();
      if (device.isConnected)
         this->anyConnected = true;
   } while (++i < std::extent<decltype(this->gamepads)>::value);
};