#include "PlayerCharacter.h"

namespace RE {
   const bool*  bPlayerInVanityMode = (bool*)  0x00B3BB04;
   const float* fVanityCurDistance  = (float* )0x00B3BB24;
   // 0x00B3BB05 may be an "auto vanity" bool, but auto vanity also sets 0x00B3BB04 to true
};