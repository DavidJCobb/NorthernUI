#pragma once
#include "ReverseEngineered/Systems/Timing.h"

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
   //
   void SetFromTime(RE::TimeGlobals* time);
};