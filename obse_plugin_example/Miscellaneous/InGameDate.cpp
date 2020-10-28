#include "InGameDate.h"
#include "ReverseEngineered/GameSettings.h"
#include "Services/Translations.h"

void InGameDate::SetFromTime(RE::TimeGlobals* timeGlobs) {
   float time = CALL_MEMBER_FN(timeGlobs, GetGameHour)();
   this->rawGameHour = time;
   this->hour = (SInt32)time;
   time = (time - this->hour) * 60.0F;
   this->minute = (SInt32)time;
   time = (time - this->minute) * 60.0F;
   this->second = (SInt32)time;
   //
   // To some extent, these getter calls are redundant; for example, we should 
   // get the weekday name from the weekday number ourselves, and we should get 
   // the season ourselves based on the month number. This is just code to get 
   // us off the ground and we can always optimize later if we need to.
   //
   this->weekday = CALL_MEMBER_FN(timeGlobs, GetGameWeekdayNum)();
   this->date    = CALL_MEMBER_FN(timeGlobs, GetGameDate)();
   this->month   = CALL_MEMBER_FN(timeGlobs, GetGameMonthNum)();
   this->year    = CALL_MEMBER_FN(timeGlobs, GetGameYear)();
};

bool InGameDate::operator==(const InGameDate& other) const {
   if (this->rawGameHour != other.rawGameHour)
      return false;
   if (this->hour != other.hour)
      return false;
   if (this->minute != other.minute)
      return false;
   if (this->second != other.second)
      return false;
   if (this->month != other.month) // also counts as checking season
      return false;
   if (this->date != other.date)
      return false;
   if (this->year != other.year)
      return false;
   if (this->weekday != other.weekday) // based on GameDaysPassed, not the real date, so check it
      return false;
   return true;
};
bool InGameDate::operator!=(const InGameDate& other) const {
   return !(*this == other);
};
SInt32 InGameDate::FormatField(char* out, UInt32 width, char type, char field) const {
   //
   // Use static variables to store some of the translateable strings, so that we don't 
   // have to query an unordered map every single frame (i.e. for XXNHUDClockMenu).
   //
   static const NorthernUI::L10N::Str* const ordinalsLower[5] = {
      &NorthernUI::L10N::sOrdinalSuffixLower0,
      &NorthernUI::L10N::sOrdinalSuffixLower1,
      &NorthernUI::L10N::sOrdinalSuffixLower2,
      &NorthernUI::L10N::sOrdinalSuffixLower3,
      &NorthernUI::L10N::sOrdinalSuffixLower4,
   };
   static const NorthernUI::L10N::Str* const ordinalsUpper[5] = {
      &NorthernUI::L10N::sOrdinalSuffixUpper0,
      &NorthernUI::L10N::sOrdinalSuffixUpper1,
      &NorthernUI::L10N::sOrdinalSuffixUpper2,
      &NorthernUI::L10N::sOrdinalSuffixUpper3,
      &NorthernUI::L10N::sOrdinalSuffixUpper4,
   };
   //
   UInt32      number = 0;
   const char* string = nullptr;
   bool        uppercase = type == 'S';
   switch (field) {
      case 'w': // weekday
         number = this->weekday;
         if (number < 7) {
            auto setting = RE::GMST::sWeekdayNames[number];
            if (setting)
               string = setting->s;
         }
         break;
      case 'D': // date
         number = this->date;
         {
            constexpr int count = std::extent<decltype(ordinalsLower)>::value;
            auto digit  = number % 10;
            auto suffix = (uppercase ? ordinalsUpper : ordinalsLower)[(digit < count ? digit : count - 1)]->value;
            if (suffix && suffix[0])
               string = suffix;
         }
         break;
      case 'M': // month
         number = this->month;
         if (number < 12) {
            auto setting = RE::GMST::sMonthNames[number];
            if (setting)
               string = setting->s;
         }
         break;
      case 'Y': // year
         number = this->year;
         break;
      case 'H': // 24-hour
         number = this->hour;
         break;
      case 'h': // 12-hour
         number = this->hour;
         string = (uppercase ? &NorthernUI::L10N::sTimeSuffixUpperAM : &NorthernUI::L10N::sTimeSuffixLowerAM)->value;
         if (number >= 12) {
            string = (uppercase ? &NorthernUI::L10N::sTimeSuffixUpperPM : &NorthernUI::L10N::sTimeSuffixLowerPM)->value;
            number = number % 12;
         }
         if (number == 0)
            number = 12;
         break;
      case 'm': // minute
         number = this->minute;
         break;
      case 's': // second
         number = this->second;
         break;
      case 'S': // season
         number = this->GetSeason();
         if (number < 4) {
            auto setting = RE::GMST::sSeasonNames[number];
            if (setting)
               string = setting->s;
         }
         break;
      default:
         return 0;
   }
   if (type == 'i' || type == 'I' || !string) {
      return sprintf(out, "%0.*u", width, number);
   } else if (type == 's' || type == 'S') {
      return sprintf(out, "%s", string);
   }
};
void InGameDate::Format(char out[256], const char* format) const {
   bool isPercent = false;
   //
   // SPEC:
   //
   // "Sundas, 17th of Last Seed" == "%sw, %2id%sd of %sm"
   // "Sundas 03 Last Seed"       == "%sw %id %sm"
   // "03/08 3E427"               == "%2id/%2im 3E%iy"
   // "2:13 pm"                   == "%ih:%2im %sh"
   // "2:13 PM"                   == "%ih:%2im %Sh"
   // "asdf % asdf %"             == "asdf %% asdf %%"
   //
   // %[width][type][value]
   //  - The width is optional and sets a minimum width for integer values. Values are padded with zero.
   //
   //  - The type may be 'i' for integer or 's' for string. Defaults to i.
   //
   //  - The value is case-sensitive; codes are as follows:
   //
   //    CODE | VALUE   | Notes
   //    w    | Weekday | Number is the weekday number from 0 to 6.
   //    D    | Date    | Stringified date is the suffix, e.g. "st", "nd", "rd", "th"
   //    M    | Month   | Number is the month number from 1 to 12.
   //    Y    | Year    | Does not include the 3E prefix.
   //    h    | Hour    | 12-hour time. Stringified value is the suffix, i.e. "am" or "pm". Type case determines suffix case, e.g. 's' -> "am", 'S' -> "AM"
   //    H    | Hour    | 24-hour time.
   //    m    | Minute  | 
   //    s    | Second  | 
   //    S    | Season  | Spring, Summer, Fall, Winter
   //
   char c = format[0]; // current character
   if (!c) {
      out[0] = '\0';
      return;
   }
   UInt32 i = 0; // index within the format string
   UInt32 j = 0; // index within the output string
   SInt32 codeStart = -1;   // index at which the code started
   UInt32 codeWidth = 0;    // format code width
   char   codeType  = '\0'; // format code type
   do {
      if (isPercent) {
         if (c == '%') {
            isPercent = false;
            out[j] = '%';
            j++;
            continue;
         }
         if (!codeType) { // handle format code width and type
            if (c >= '0' && c <= '9') {
               codeWidth *= 10;
               codeWidth += (c - '0');
               continue;
            }
            codeType = c;
            if (c != 'i' && c != 's' && c != 'I' && c != 'S')
               goto code_is_bad;
            continue;
         }
         {  // handle format code field, i.e. actually write the final value
            UInt32 result = this->FormatField(out + j, codeWidth, codeType, c);
            if (result <= 0)
               goto code_is_bad;
            j += result;
            isPercent = false;
            continue;
         }
      code_is_bad:
         //
         // Just print the bad format code.
         //
         for (UInt32 k = codeStart; k < i; k++) {
            out[j] = format[k];
            j++;
         }
         out[j] = c;
         j++;
         isPercent = false;
         continue;
      } else {
         if (c == '%') {
            isPercent = true;
            codeStart = i;
            codeWidth = 0;
            codeType  = '\0';
            continue;
         }
         out[j] = c;
         j++;
      }
   } while ((c = format[++i]) && j < 255);
   out[j] = '\0';
};
UInt8 InGameDate::GetSeason() const noexcept {
   switch (this->month) {
      case 11:
      case 0:
      case 1:
         return 3;
      case 2:
      case 3:
      case 4:
         return 0;
      case 5:
      case 6:
      case 7:
         return 1;
      case 8:
      case 9:
      case 10:
         return 2;
   }
   return 0;
};