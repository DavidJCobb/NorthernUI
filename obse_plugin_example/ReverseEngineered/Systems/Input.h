#pragma once
#include "obse/Utilities.h"
#include "Core.h"

#include <dinput.h>

namespace RE {
   const auto JoystickDeviceObjFmt = (DIOBJECTDATAFORMAT*) 0x006B9FA0; // defines DIJOYSTATE as the device state structure for joysticks
   const auto JoystickDeviceFormat = (DIDATAFORMAT*) 0x00A78C54; // { 0x18, 0x10, DIDF_ABSAXIS, 0x50, 0x2C, JoystickDeviceObjFmt };

   enum KeyQuery : UInt8 {
      kKeyQuery_Hold = 0, // key is down
      kKeyQuery_Down = 1, // key just went down
      kKeyQuery_Up = 2, // key just went up
      kKeyQuery_Change = 3, // key just went up or down
      kKeyQuery_Double = 4, // key was double-clicked (for mouse buttons 0 - 7 only)
   };
   enum JoystickControl : UInt8 {
      kJoystickControl_UnkButton00 = 0x00,
      //
      // ... buttons range up to, and do not include, 0x20
      //
      kJoystickControl_UnkControl20 = 0x20,
   };
   enum MouseAxis : UInt8 {
      kMouseAxis_X = 1,
      kMouseAxis_Y = 2,
      kMouseAxis_Scroll = 3,
   };
   enum MouseControl : UInt8 {
      kMouseControl_Left = 0,
      kMouseControl_Right = 1,
      kMouseControl_UnkButton2 = 2,
      kMouseControl_UnkButton3 = 3,
      kMouseControl_UnkButton4 = 4,
      kMouseControl_UnkButton5 = 5,
      kMouseControl_UnkButton6 = 6,
      kMouseControl_UnkButton7 = 7,
      kMouseControl_WheelUp = 8, // could have these two backwards
      kMouseControl_WheelDown = 9, //
   };
   enum MappableControl : UInt8 {
      kControl_Forward = 0x00,
      kControl_Back = 0x01,
      kControl_StrafeLeft = 0x02,
      kControl_StrafeRight = 0x03,
      kControl_UseEquipped = 0x04,
      kControl_Activate = 0x05,
      kControl_Block,
      kControl_Cast,
      kControl_DrawSheatheWeapon,
      kControl_Sneak,
      kControl_Run = 0x0A,
      kControl_AutoRun,
      kControl_AutoMove,
      kControl_Jump,
      kControl_TogglePOV,
      kControl_MenuMode = 0x0F,
      kControl_Rest = 0x10,
      kControl_QuickMenu = 0x11,
      kControl_Quick1 = 0x12,
      kControl_Quick2,
      kControl_Quick3,
      kControl_Quick4,
      kControl_Quick5,
      kControl_Quick6,
      kControl_Quick7,
      kControl_Quick8 = 0x19,
      kControl_QuickSave = 0x1A,
      kControl_QuickLoad = 0x1B,
      kControl_Grab = 0x1C,
      kControl_MAXSTANDARD = kControl_Grab,
      kControl_COUNT,
      //
      // Special indices follow. These are not represented in the Scheme data.
      //
      kControl_SpecialPause = 0x1D, // QueryControlState checks keyboard state directly for DIK_ESCAPE
      kControl_SpecialConsole = 0x1E, // QueryControlState checks keyboard state directly for DIK_GRAVE
      kControl_SpecialPrintScreen = 0x1F  // QueryControlState checks keyboard state directly for DIK_SYSRQ
   };

   class OSInputGlobals {
      public:
         enum {
            kFlag_HasJoysticks    = 1 << 0,
            kFlag_HasMouse        = 1 << 1,
            kFlag_HasKeyboard     = 1 << 2,
            kFlag_BackgroundMouse = 1 << 3,
         };
         enum {
            kMaxDevices = 8,
         };

         OSInputGlobals();
         ~OSInputGlobals();

         struct JoystickObjectsInfo {
            enum {
               kHasXAxis = 1 << 0,
               kHasYAxis = 1 << 1,
               kHasZAxis = 1 << 2,
               kHasXRotAxis = 1 << 3,
               kHasYRotAxis = 1 << 4,
               kHasZRotAxis = 1 << 5
            };
            //
            UInt32 axis;
            UInt32 buttons;
         };
         struct Unk1AF4 { // sizeof == 0x2C
            UInt32 bufLen;
            UInt8  unk04[0x2C - 4];
         };
         struct JoystickDeviceState { // sizeof == 0x70 // but GetJoystickAxisMovement suggests it should be 0xA0??
            DIJOYSTATE allStateThisFrame;    // 00 // sizeof == 0x50
            UInt8      buttonsLastFrame[32]; // 50 // sizeof == 0x20
         };
         struct Scheme { // sizeof == 0x1D
            //
            // These use DXScanCodes and match controls to their assigned keycodes.
            //
            UInt8  bindings[kControl_COUNT];
            inline UInt8& operator[] (int control) { // allow myScheme[0] == myScheme->ctrlForward, since Bethesda's own code treats it as a UInt8[]
               return this->bindings[control];
            }
         };

         UInt32               flags;                      // 0000
         IDirectInput8*       dinputInterface;            // 0004 // result of DirectInput8Create call (which is itself hooked by Steam and OBSE, in that order) in constructor
         IDirectInputDevice8* dxDeviceJoysticks[8];       // 0008 // format is c_dfDIJoystick
         IDirectInputDevice8* dxDeviceKeyboard;           // 0028 // format is c_dfDIKeyboard
         IDirectInputDevice8* dxDeviceMouse;              // 002C // format is c_dfDIMouse2
         JoystickDeviceState  joystickDeviceStates[8];    // 0030
         UInt8                unk03B0[(0x530 - 0x3B0)];   // 03B0 // bytes set to 0 via memset in the constructor // related to joysticks
         DIDEVICEINSTANCE     joystickDeviceInstances[8]; // 0530
         DIDEVCAPS            joystickCapabilities[8];    // 1750 // sizeof each == 0x2C
         JoystickObjectsInfo	joystickAvailableCtrls[8];  // 18B0
         UInt32               joystickCount = 0;          // 18F0 // capped to 8
         //
         UInt8   keyboardDeviceStateThisFrame[0x100]; // 18F4
         UInt8   keyboardDeviceStateLastFrame[0x100]; // 19F4
         Unk1AF4 unk1AF4;  // 1AF4
         DIMOUSESTATE2 mouseDeviceStateThisFrame; // 1B20
         DIMOUSESTATE2 mouseDeviceStateLastFrame; // 1B34
         UInt32 swapMouseButton; // 1B48 // Windows API USER32.SwapMouseButton()
         UInt32 doubleClickTime; // 1B4C // Windows API USER32.GetDoubleClickTime()
         UInt8  unk1B50[8];      // 1B50 // related to double-click handling; see 00403C30 // consecutive click count?
         UInt32 unk1B58[8];      // 1B58 // related to double-click handling; see 00403C30 // time of last click, in ticks, per button?
         UInt32 unk1B78; // 1B78
         UInt16 unk1B7C; // 1B7C
         //
         // INI settings map to a control as AAAABBBB where AAAA is the 
         // UInt16 for keyboard and BBBB is the UInt16 for mouse. Note 
         // that the codes are stored as UInt8 in memory, and 0xFF is 
         // used as a sentinel to indicate that a key is not bound.
         //
         Scheme keyboard; // 1B7E
         Scheme mouse;    // 1B9B
         Scheme joystick; // 1BB8 // any device that isn't a keyboard or mouse; keycodes line up with JoystickData above
         //
         UInt8  pad1BD5;  // 1BD5
         UInt8  pad1BD6;  // 1BD6
         UInt8  pad1BD7;  // 1BD7

         static OSInputGlobals* GetInstance() {
            auto core = OSGlobals::GetInstance();
            if (!core)
               return nullptr;
            return core->input;
         };
         //
         MEMBER_FN_PREFIX(OSInputGlobals);
         DEFINE_MEMBER_FN(FlushKeyboardBuffer,        void,   0x00403160);
         DEFINE_MEMBER_FN(GetJoystickAxisMovement,    SInt32, 0x00402F50, UInt8 whichJoystick, UInt8 whichAxis); // axis indices: 1, 2, 3, 4, 5, 6: x, y, z, rX, rY, rZ // axes are normalized to the range of [-100, 100]
         DEFINE_MEMBER_FN(GetJoystickButtonCount,     UInt32, 0x004030B0, UInt8 whichJoystick);
         DEFINE_MEMBER_FN(GetJoystickPOVControlCount, UInt32, 0x004030D0, UInt8 whichJoystick);
         DEFINE_MEMBER_FN(GetMouseAxisMovement,       SInt32, 0x00403190, UInt8 whichAxis); // 1, 2, 3: x, y, z; return values are pixels moved since last tick
         DEFINE_MEMBER_FN(QueryControlState,          UInt32, 0x00403520, MappableControl controlIndex, KeyQuery query); // returns the number of keys that are mapped to that control and match the KeyQuery
            //  - loops over each scheme and calls QueryInputState for the key the control is mapped to
            //  - for special keycodes (1D, 1E, 1F), skips that and uses special fields
         DEFINE_MEMBER_FN(QueryInputState,            UInt32, 0x00403490, UInt8 schemeIndex, UInt8 keycode, KeyQuery query);
            //  - if schemeIndex > 1, then (schemeIndex - 2) is the joystick index to use
            //  - for keyboard and mouse, just calls QueryKeyboardKeyState and QueryMouseKeyState
            //  - if we wanna add gamepad support, we'd need to patch this to query XInput for state
         DEFINE_MEMBER_FN(QueryJoystickButtonState,   bool,   0x00402FC0, UInt8 whichJoystick, UInt8 buttonIndex, KeyQuery query);
         DEFINE_MEMBER_FN(QueryKeyboardKeyState,      bool,   0x004032D0, UInt8 keycode, KeyQuery query);
         DEFINE_MEMBER_FN(QueryMouseKeyState,         bool,   0x004031E0, MouseControl keycode, KeyQuery query); // for scroll wheels, always acts as if query is Hold
         DEFINE_MEMBER_FN(RebindControl,              bool,   0x00403F50, UInt8 whichCtrl, UInt8 whichScheme, UInt8 newButton); // forbids use of Esc, Tilde, Print Screen, F1 - F4, and 1 - 8
         DEFINE_MEMBER_FN(RebindControlMinimalChecks, bool,   0x00403B80, UInt8 whichCtrl, UInt8 whichScheme, UInt8 newButton); // forbids use of Esc, Tilde, and Print Screen
         DEFINE_MEMBER_FN(ResetControlMap,            void,   0x00403960, UInt8 whichScheme); // 0, 1, 2, 3 == keyboard, mouse, unk1BB8, all
         DEFINE_MEMBER_FN(SendControlPress,           UInt8*, 0x00403380, MappableControl whichControl); // forces the control to be "pressed" for the first scheme in which it is defined: keyboard, mouse, joystick // only works for controls < 0xE, i.e. controls related to movement and gameplay
         DEFINE_MEMBER_FN(SetJoystickDeadzone,        void,   0x004030F0, UInt8 whichJoystick, float deadzonePercent);
         //
         DEFINE_MEMBER_FN(PollAndUpdateInputState,    void,   0x004046A0);
            //  - called (indirectly) by the game's main loop
            //  - updates the "state this frame" and "state last frame" vars
            //  - also calls a function to update double-click state
            //  - if we wanna add gamepad support, we'd need to patch this to poll XInput
   };
   static_assert(sizeof(OSInputGlobals) >= 0x1BD8, "RE::OSInputGlobals is too small!");
   static_assert(sizeof(OSInputGlobals) <= 0x1BD8, "RE::OSInputGlobals is too large!");
   static_assert(offsetof(OSInputGlobals, swapMouseButton) >= 0x1B48, "RE::OSInputGlobals::swapMouseButton is too early!");
   static_assert(offsetof(OSInputGlobals, swapMouseButton) <= 0x1B48, "RE::OSInputGlobals::swapMouseButton is too late!");
   static_assert(offsetof(OSInputGlobals, keyboard) >= 0x1B7E, "RE::OSInputGlobals::keyboard is too early!");
   static_assert(offsetof(OSInputGlobals, keyboard) <= 0x1B7E, "RE::OSInputGlobals::keyboard is too late!");
   static_assert(offsetof(OSInputGlobals, mouse) >= 0x1B9B, "RE::OSInputGlobals::mouse is too early!");
   static_assert(offsetof(OSInputGlobals, mouse) <= 0x1B9B, "RE::OSInputGlobals::mouse is too late!");
   static_assert(offsetof(OSInputGlobals, joystick) >= 0x1BB8, "RE::OSInputGlobals::joystick is too early!");
   static_assert(offsetof(OSInputGlobals, joystick) <= 0x1BB8, "RE::OSInputGlobals::joystick is too late!");
};