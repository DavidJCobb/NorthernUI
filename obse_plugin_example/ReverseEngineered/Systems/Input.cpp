#include "Input.h"

namespace RE {
   extern const char** const g_KeyNames = (const char**) 0x00B39578;

   extern UInt8 g_defaultJoystickMappings[0x1D] = {
      0xFF, // 00
      0xFF,
      0xFF,
      0xFF,
      9, // 04: Use Equipped
      3, // 05: Activate
      8, // 06: Block
      6, // 07: Cast
      2, // 08: Draw/Sheathe Weapon
      0, // 09: Sneak
      0xFF, // 0A: Run
      0xFF, // 0B
      0xFF, // 0C
      1, // 0D: Jump
      5, // 0E: Toggle POV
      4, // 0F: Menu Mode
      0x0A, // 10: Rest
      0xFF,
      0x20,
      0x21,
      0x22,
      0x23,
      0x24,
      0x25,
      0x26,
      0x27,
      0xFF,
      0xFF,
      7, // 1C: Grab
   };
};