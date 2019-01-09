#pragma once

namespace cobb {
   extern bool string_has_content(const char* str); // any characters besides '\0' and std::isspace-positive glyphs
   extern bool string_says_false(const char* str); // the string spells the case-insensitive word "false", ignoring whitespace
   extern bool string_to_int(const char* str, SInt32& out, bool allowHexOrDecimal = false);  // returns true if it's a valid integer and no non-whitespace follows the number; out is not modified otherwise
   extern bool string_to_float(const char* str, float& out); // returns true if it's a valid float   and no non-whitespace follows the number; out is not modified otherwise
   //
   extern void replace_all_of(std::string& subject, const std::string& token, const std::string& target);
   extern std::string& ltrim(std::string& subject);
   extern std::string& rtrim(std::string& subject);
   extern std::string& trim(std::string& subject);
   extern void snprintf(std::string& out, const char* format, ...);
   extern SInt32 stricmp(const std::string& a, const std::string& b);
   extern bool strieq(const std::string& a, const std::string& b);
   extern bool striendswith(const std::string& haystack, const std::string& needle);

   //
   // Given a format like "example %s %s %d", the valid tokens would be "ssd". For 
   // "example %.*s" or "example %*d" the tokens would be "*s". Note that format 
   // tokens with multiple flags (e.g. "%- d") are not supported, unless the last 
   // flag is zero (in which case it's taken as part of the width field e.g. "%-0d").
   //
   // Note that this returns false if the format string has too many tokens or the 
   // wrong tokens (including in the wrong order). It does not return false if the 
   // format string has too few tokens, because a format string that doesn't use 
   // every parameter given shouldn't cause a crash (and in some cases, being able 
   // to do that is desirable).
   //
   extern __declspec(noinline) bool validate_format(const std::string& format, const std::string& tokens);

   extern void trim_and_explode(std::string& line, std::vector<std::string>& out);
   extern void find_quoted(std::string& line, std::string& out);

   extern SInt32 xml_numeric_entity_to_char_code(const char* xmlEntity, UInt32& outCount); // You must supply a string starting with "&#". Function returns -1 if it's not a valid entity (e.g. "&#bullcrap;" or "&#12" with no terminator or so on).
};