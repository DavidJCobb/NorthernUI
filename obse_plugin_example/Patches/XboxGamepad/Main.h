#pragma once
#include <xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")
#include "ReverseEngineered/Systems/Input.h"

#include <unordered_map>

extern HMODULE g_xInputDllHandle;
extern bool    g_xInputPatchApplied;

UInt32 XInputControlBindingToDXScanCode(const UInt32 binding);

class XXNGamepad {
   public:
      XXNGamepad();
      enum Button {
         kGamepadButton_DPadUp = 0,
         kGamepadButton_DPadDown,
         kGamepadButton_DPadLeft,
         kGamepadButton_DPadRight,
         kGamepadButton_Start, // Hard-mapped to Pause/Esc; don't let anything else get mapped to this
         kGamepadButton_Back,
         kGamepadButton_LS,
         kGamepadButton_RS,
         kGamepadButton_LB,
         kGamepadButton_RB,
         kGamepadButton_Unused0400,
         kGamepadButton_Unused0800,
         kGamepadButton_A = 0xC,
         kGamepadButton_B,
         kGamepadButton_X,
         kGamepadButton_Y,
         kGamepadButton_LastStandardButton = kGamepadButton_Y,
         kGamepadButton_SpecialCaseLT,
         kGamepadButton_SpecialCaseRT,
         //
         kGamepadButton_COUNT
      };

      UInt8          index = 0; // set by XXNGamepadSupportCore::XXNGamepadSupportCore()
      bool           isConnected = false; // assembly code assumes this is at offset 01 within the class
      XINPUT_GAMEPAD lastFrame;
      XINPUT_GAMEPAD thisFrame;
      XINPUT_GAMEPAD ignoring;
      XINPUT_GAMEPAD disabled; // anything disabled by scripts

      typedef RE::KeyQuery KeyQuery;

      enum JoystickAxis {
         //
         // Normally these would be sequential from 1 to 6, matching X, Y, Z, rX, rY, rZ; however, 
         // that doesn't line up with the default values in Oblivion.ini. The values 3, 6, x, 1, 2, x 
         // do.
         //
         kJoystickAxis_LeftX = 1,
         kJoystickAxis_LeftY = 2,
         kJoystickAxis_LeftZ = 4,
         kJoystickAxis_RightX = 3,
         kJoystickAxis_RightY = 6,
         kJoystickAxis_RightZ = 5,
      };

      void Update();

      SInt32 AnyKeyMatches(KeyQuery state) const; // returns -1 if no matches
      void   Clear();
      __declspec(noinline) bool GetButtonState(UInt8 button, KeyQuery state, bool ignoreDisabled = false) const;
      SInt32 GetJoystickAxis(UInt8 axis) const;
      void   IgnoreButtons(); // flags all currently-pressed buttons as "ignore;" see (uiIgnore)
      void   SendControlPress(UInt8 button);
      void   SetVibrationSpeeds(UInt16 speed) const;
      void   SetVibrationSpeeds(UInt16 speedL, UInt16 speedR) const;

      bool GetDisableState(UInt8 button) const;
      void SetDisableState(UInt8 button, bool disabled);

      static const char* GetButtonName(UInt8 button); // NOT localized; used internally
      static UInt32 GetButtonCode(const char* name); // NOT localized; used internally
};

struct XXNGamepadSupportCore {
   public:
      XXNGamepadSupportCore();

      XXNGamepad gamepads[4];
      bool       anyConnected = false;

   private:
      bool disabled = false;

   public:
      inline void Enable() {
         this->disabled = false;
      };
      void        Disable();
      XXNGamepad* GetAnyGamepad(); // gets the first available one
      UInt8       GetBoundKey(UInt8) const;
      XXNGamepad* GetGamepad(UInt8); // bounds-checked
      void        Update();

      __declspec(noinline) static XXNGamepadSupportCore* GetInstance();
};