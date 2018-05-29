#pragma once
#include "obse/Utilities.h"

class BSStringT;
class TESGlobal;

struct InGameDate {
   float  rawGameHour = 0.0F;
   UInt8  hour;
   UInt8  minute;
   UInt8  second;
   UInt8  weekday;
   UInt8  date;
   UInt8  month; // season isn't stored; we can deduce it from the month
   UInt16 year;
   //
   bool operator==(const InGameDate &other) const; // only compares numeric components
   bool operator!=(const InGameDate &other) const;
   //
   SInt32 FormatField(char* out, UInt32 width, char type, char field) const; // return value is the same as with sprintf
   void   Format(char out[256], const char* format) const;
   UInt8  GetSeason() const noexcept;
};

namespace RE {
   enum {
      kInGameSeason_Spring = 0,
      kInGameSeason_Summer = 1,
      kInGameSeason_Fall   = 2,
      kInGameSeason_Winter = 3,
      //
      kInGameSeason_Autumn = kInGameSeason_Fall,
   };
   struct TimeInfo {
      UInt8  disableCount;  // 00
      UInt8  pad1[3];       // 01
      float  fpsClamp;      // 04 // in seconds
      float  unk08;         // 08
      float  frameTime;     // 0C // in seconds; includes the effect of INI:General:fGlobalTimeMultiplier
      UInt32 unk10;         // 10 // in ms // very much appears to be the current executable time, i.e. if you wanted to do something every X seconds, you'd check whether (unk10 - lastTimeYouDidIt >= X).
      UInt32 unk14;         // 14 // in ms // OBSE calls this "gameStartTime" but that seems wrong?

      MEMBER_FN_PREFIX(TimeInfo);
      DEFINE_MEMBER_FN(Constructor, void, 0x0047D150, UInt32 tickCount);

      // TODO: finish looking at method 0047D170
   };
   extern TimeInfo* g_timeInfo;

   class TimeGlobals {
      public:
         TimeGlobals();
         ~TimeGlobals();

         TESGlobal* gameYear; // 00
         TESGlobal* gameMonth; // 04
         TESGlobal* gameDay; // 08
         TESGlobal* gameHour; // 0C
         TESGlobal* gameDaysPassed; // 10
         TESGlobal* timeScale; // 14

         void GetInGameDate(InGameDate* out);
         UInt32 GetGameHourAndMinutes(); // returns time as 0xHHHHMMMM. just wrote this as a demonstration

         MEMBER_FN_PREFIX(TimeGlobals);
         DEFINE_MEMBER_FN(FillGlobalPointers, void,        0x00402860); // checks hardcoded form IDs to get the TESGlobals the object needs
         DEFINE_MEMBER_FN(FormatGameDate,     BSStringT*,  0x00402E50, BSStringT* out); // "%s %d, 3E%d" given month, date, year
         DEFINE_MEMBER_FN(GetDaysPassed,      UInt32,      0x00402C00);
         DEFINE_MEMBER_FN(GetGameDate,        UInt32,      0x00402BA0);
         DEFINE_MEMBER_FN(GetGameHour,        float,       0x00402BD0); // returns hour and minute as a single float, measured in hours, e.g. 6:30am == 6.5
         DEFINE_MEMBER_FN(GetGameMonthNum,    UInt32,      0x00402B80);
         DEFINE_MEMBER_FN(GetGameSeasonNum,   UInt32,      0x00402DC0); // 0, 1, 2, or 3 depending on the in-game month
         DEFINE_MEMBER_FN(GetGameWeekday,     const char*, 0x00402C90);
         DEFINE_MEMBER_FN(GetGameWeekdayNum,  UInt32,      0x00402C40);
         DEFINE_MEMBER_FN(GetGameYear,        UInt32,      0x00402B40);
         DEFINE_MEMBER_FN(GetTimescale,       float,       0x004029D0);
         DEFINE_MEMBER_FN(Update,             void,        0x004029E0, float seconds); // advances the in-game time by the specified number of in-game seconds, updating globals as needed
   };
   extern TimeGlobals* g_timeGlobals;
};