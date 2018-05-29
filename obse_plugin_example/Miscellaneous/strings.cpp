#include "strings.h"
#include <algorithm>
#include <cctype>

namespace cobb {
   bool string_has_content(const char* str) {
      char c = *str;
      while (c) {
         if (!std::isspace(c))
            return true;
         str++;
         c = *str;
      }
      return false;
   }
   bool string_says_false(const char* str) {
      char c = *str;
      while (c) { // skip leading whitespace
         if (!std::isspace(c))
            break;
         str++;
         c = *str;
      }
      if (c) {
         UInt8 i = 0;
         do {
            char d = ("FALSEfalse")[i + (c > 'Z' ? 5 : 0)];
            if (c != d)
               return false;
            str++;
            c = *str;
         } while (++i < 5);
         //
         // Ignoring leading whitespace, the string starts with "FALSE". Return true if no non-whitespace 
         // chars follow that.
         //
         while (c) {
            if (!std::isspace(c))
               return false;
            str++;
            c = *str;
         }
         return true;
      }
      return false;
   };
   bool string_to_float(const char* str, float& out) {
      errno = 0;
      char* end = nullptr;
      float o = strtof(str, &end);
      if (end == str) // not a number
         return false;
      {  // if any non-whitespace chars after the found value, then the string isn't really a number
         char c = *end;
         while (c) {
            if (!isspace(c))
               return false;
            ++end;
            c = *end;
         }
      }
      if (errno == ERANGE) // out of range
         return false;
      out = o;
      return true;
   };
   bool string_to_int(const char* str, SInt32& out) {
      errno = 0;
      char* end = nullptr;
      SInt32 o = strtol(str, &end, 10);
      if (end == str) // not a number
         return false;
      {  // if any non-whitespace chars after the found value, then the string isn't really a number
         char c = *end;
         while (c) {
            if (!isspace(c))
               return false;
            ++end;
            c = *end;
         }
      }
      if (errno == ERANGE) // out of range
         return false;
      out = o;
      return true;
      //
      /*// Code retained because it was fun to write:
      UInt32 i = 0;
      SInt32 value = 0;
      bool   validNum = false;
      bool   foundMinus = false;
      char c = str[i];
      if (!c)
         return false;
      do {
         if (c == '=')
            break;
         if (c == '-') {
            if (foundMinus || validNum) {
               validNum = false;
               break;
            }
            foundMinus = true;
            continue;
         }
         if (c >= '0' && c <= '9') {
            if (foundMinus) {
               validNum = false;
               break;
            }
            validNum = true;
            value = (value * 10) + (c - '0');
         } else {
            validNum = false;
            break;
         }
      } while (c = str[++i]);
      if (validNum)
         out = value;
      return validNum;*/
   };

   void replace_all_of(std::string& subject, const std::string& token, const std::string& target) {
      auto t = token.size();
      auto l = target.size();
      auto i = subject.find(token);
      while (i != std::string::npos && i < subject.size()) {
         subject.replace(i, t, target);
         i = subject.find(token, i + l);
      }
   };
   std::string& ltrim(std::string& subject) {
      subject.erase(
         subject.begin(),
         std::find_if(
            subject.begin(),
            subject.end(),
            [](int c) {
               return !std::isspace(c);
            }
         )
      );
      return subject;
   };
   std::string& rtrim(std::string& subject) {
      subject.erase(
         std::find_if(
            subject.rbegin(),
            subject.rend(),
            [](int c) { return !std::isspace(c); }
         ).base(),
         subject.end()
      );
      return subject;
   };
   std::string& trim(std::string& subject) {
      return rtrim(ltrim(subject));
   };
   void snprintf(std::string& out, const char* format, ...) {
      va_list args;
      va_start(args, format);
      va_list safe;
      va_copy(safe, args);
      {
         char b[128];
         if (vsnprintf(b, sizeof(b), format, args) < 128) {
            out = b;
            va_end(safe);
            va_end(args);
            return;
         }
      }
      UInt32 s = 256;
      char*  b = (char*) malloc(s);
      UInt32 r = vsprintf(b, format, args);
      while (r > s) {
         va_copy(args, safe);
         s += 20;
         delete b;
         char*  b = (char*)malloc(s);
         r = vsprintf(b, format, args);
      }
      out  = b;
      delete b;
      va_end(safe);
      va_end(args);
   };
   SInt32 stricmp(const std::string& a, const std::string& b) {
      size_t sA = a.size();
      size_t sB = b.size();
      size_t s = (std::min)(sA, sB);
      for (size_t i = 0; i < s; i++) {
         unsigned char c = tolower(a[i]);
         unsigned char d = tolower(b[i]);
         if (c != d)
            return (SInt32)(UInt32)c - d;
      }
      if (sA == sB)
         return 0;
      return (SInt32)sA - sB;
   };
   bool strieq(const std::string& a, const std::string& b) {
      return std::equal(
         a.begin(), a.end(),
         b.begin(), b.end(),
         [](char a, char b) {
            return tolower(a) == tolower(b);
         }
      );
      /*size_t s;
      {
         size_t sA = a.size();
         size_t sB = b.size();
         if (sA != sB)
            return false;
         s = (std::min)(sA, sB);
      }
      for (size_t i = 0; i < s; i++) {
         char c = tolower(a[i]);
         char d = tolower(b[i]);
         if (c != d)
            return false;
      }
      return 0;*/
   };

   __declspec(noinline) bool validate_format(const std::string& format, const std::string& tokens) {
      auto   sf = format.size();
      auto   st = tokens.size();
      size_t token = 0;
      size_t i  = 0;
      for (size_t i = 0; i < sf; i++) {
         if (format[i] != '%')
            continue;
         if (i + 1 < sf && format[i + 1] == '%') { // skip %% escapes
            i++;
            continue;
         }
         //
         struct Token {
            enum State : UInt8 {
               kState_NotFound,
               kState_InProgress,
               kState_After
            };
            State  state = kState_NotFound;
            size_t index = std::string::npos;
         };
         bool  isValid = true;
         Token flags;
         Token width;
         Token precision;
         Token length;
         Token type;
         auto j = i + 1;
         do {
            char c = format[j];
            if (length.state == Token::kState_InProgress) {
               switch (c) {
                  case 'h':
                  case 'l':
                     flags.state = width.state = precision.state = length.state = Token::kState_After;
                     continue;
               }
            } else if (length.state == Token::kState_NotFound) {
               switch (c) {
                  case 'h':
                  case 'l':
                     flags.state = width.state = precision.state = Token::kState_After;
                     length.state = Token::kState_InProgress;
                     length.index = j;
                     continue;
                  case 'L':
                  case 'j':
                  case 't':
                  case 'z':
                     flags.state = width.state = precision.state = Token::kState_After;
                     length.state = Token::kState_After;
                     length.index = j;
                     continue;
               }
            }
            if (precision.state == Token::kState_NotFound && c == '.') {
               flags.state = width.state = Token::kState_After;
               precision.state = Token::kState_InProgress;
               precision.index = j;
               continue;
            } else if (precision.state == Token::kState_InProgress) {
               if (c >= '0' && c <= '9')
                  continue;
               if (c == '*') {
                  precision.state = Token::kState_After;
                  continue;
               }
            }
            if (width.state == Token::kState_NotFound || width.state == Token::kState_InProgress) {
               if (c >= '0' && c <= '9') {
                  width.state = Token::kState_InProgress;
                  width.index = j;
                  continue;
               }
               if (width.state == Token::kState_NotFound && c == '*') {
                  width.state = Token::kState_After;
                  width.index = j;
                  continue;
               }
            } else if (width.state == Token::kState_InProgress) {
               if (c >= '0' && c <= '9')
                  continue;
            }
            if (flags.state != Token::kState_After) {
               switch (c) {
                  case ' ':
                  case '-':
                  case '+':
                  case '#':
                  case '0': // width would catch this
                     flags.state = Token::kState_After; // TODO: this only supports one flag on a token
                     flags.index = j;
                     continue;
               }
            }
            if (type.state == Token::kState_NotFound) {
               switch (c) {
                  case 'a':
                  case 'A':
                  case 'c':
                  case 'd':
                  case 'e':
                  case 'E':
                  case 'f':
                  case 'F':
                  case 'g':
                  case 'G':
                  case 'i':
                  case 'o':
                  case 's':
                  case 'u':
                  case 'x':
                  case 'X':
                     type.state = Token::kState_After;
                     type.index = j;
                     break;
               }
               if (type.state != Token::kState_NotFound)
                  break;
            }
            isValid = false;
            break;
         } while (++j < sf);
         if (isValid && type.state == Token::kState_After) { // latter check needed in case string ended prematurely
            if (width.index != std::string::npos) {
               if (format[width.index] == '*') {
                  if (tokens[token] != '*')
                     return false;
                  token++;
               }
            }
            if (precision.index != std::string::npos) {
               if (format[precision.index + 1] == '*') {
                  if (tokens[token] != '*')
                     return false;
                  token++;
               }
            }
            char current = format[type.index];
            char valid   = tokens[token];
            if (valid == '*')
               return false;
            if (current == 's' || current == 'c' || valid == 's' || valid == 'c') {
               if (valid != current)
                  return false;
            }
            token++;
         }
      }
      if (token > st) // string demands too many tokens
         return false;
      return true;
   };
}