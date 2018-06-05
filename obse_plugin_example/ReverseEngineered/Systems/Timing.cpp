#include "Timing.h"
#include "ReverseEngineered/GameSettings.h"
#include "Services/Translations.h"

namespace RE {
   TimeInfo*    g_timeInfo    = (TimeInfo*)    0x00B33E90;
   TimeGlobals* g_timeGlobals = (TimeGlobals*) 0x00B332E0;
   //
   UInt32 TimeGlobals::GetGameHourAndMinutes() {
      float raw = CALL_MEMBER_FN(this, GetGameHour)();
      SInt32 result = (SInt32)raw; // hours
      float minutes = raw - result;
      result = result << 0x10;
      minutes *= 60;
      result |= (std::min)((signed int)minutes, 60);
      return result;
   };
};