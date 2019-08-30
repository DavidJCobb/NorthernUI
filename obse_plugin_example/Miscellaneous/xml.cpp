#include "xml.h"
#include "strings.h"

namespace {
   constexpr char* entities[] = {
      "&amp;",
      "&apos;",
      "&gt;",
      "&lt;",
      "&nbsp;",
      "&newline;",
      "&quot;",
      "&minus;",
   };
   constexpr char  results[] = {
      '&',
      '\'',
      '>',
      '<',
      ' ',
      '\n',
      '"',
      '-',
   };
   static_assert(std::extent<decltype(entities)>::value == std::extent<decltype(results)>::value, "List of allowed XML entities and list of XML entity replacement strings are mismatched!");

   // returns number of chars skipped
   UInt32 _handleXMLEntity(const cobb::XMLDocument& doc, const char* ampersand, std::string& writeTo, UInt32 lineNumber, UInt32 colNumber) {
      char d = ampersand[1];
      char e = d ? ampersand[2] : '\0';
      if (d == '#') {
         const char* offset = ampersand + 2;
         UInt32 base = 10;
         if (e == 'x') {
            base = 16;
            offset++;
         }
         char* end = nullptr;
         UInt32 number = strtoul(offset, &end, base);
         if ((end == offset) || (end && *end != ';')) {
            _MESSAGE("XML parse error on line %d column %d: found a '&' glyph that isn't a valid character entity.", lineNumber, colNumber);
            return 0;
         }
         if (number == 0) {
            _MESSAGE("XML parse error on line %d column %d: null characters are not allowed in this document even when escaped using XML entities.", lineNumber, colNumber);
            return 0;
         }
         if (number > 255) {
            _MESSAGE("XML parse warning on line %d column %d: character entity uses code %d, which is out-of-range; Oblivion uses single-byte text. Skipping this entity.", lineNumber, colNumber, number);
         } else {
            writeTo += (char)number;
         }
         return end - ampersand;
      }
      for (auto it = doc.entities.begin(); it != doc.entities.end(); ++it) {
         auto& entity = it->entity;
         auto  length = entity.size();
         if (strncmp(ampersand, entity.c_str(), length) == 0) {
            writeTo += it->substitution;
            return length;
         }
      }
      for (UInt32 i = 0; i < std::extent<decltype(entities)>::value; i++) {
         auto entity = entities[i];
         auto length = strlen(entity);
         if (strncmp(ampersand, entity, length) == 0) {
            writeTo += results[i];
            return length;
         }
      }
      _MESSAGE("XML parse error on line %d column %d: found a '&' glyph that isn't a valid entity.", lineNumber, colNumber);
      return 0;
   }
};

namespace cobb {
   void XMLDocument::clear() {
      this->tokens.clear();
   }
   bool XMLDocument::meetsStrictRequirements() const {
      bool   foundRoot = false;
      UInt32 nesting = 0;
      for (auto it = this->tokens.begin(); it != this->tokens.end(); ++it) {
         auto& token = *it;
         switch (token.code) {
            case kXMLToken_ElementOpen:
               if (nesting == 0 && foundRoot) {
                  //
                  // ERROR: A document can only have one root element.
                  //
                  return false;
               }
               ++nesting;
               foundRoot = true;
               break;
            case kXMLToken_ElementClose:
               --nesting;
               break;
         }
      }
      return true;
   }

   void XMLDocument::defineEntity(const char* entity, const char* substitution) {
      this->entities.emplace_back(entity, substitution);
   }

   void XMLDocument::addElementStart(UInt32 line, std::string& tagName) {
      this->tokens.emplace_back(line, kXMLToken_ElementOpen);
      auto last = this->tokens.rbegin();
      std::swap(last->name, tagName);
   }
   void XMLDocument::addElementEnd(UInt32 line, std::string& tagName) {
      this->tokens.emplace_back(line, kXMLToken_ElementClose);
      auto last = this->tokens.rbegin();
      std::swap(last->name, tagName);
   }
   void XMLDocument::addElementAttribute(UInt32 line, std::string& name, std::string& value) {
      this->tokens.emplace_back(line, kXMLToken_Attribute);
      auto last = this->tokens.rbegin();
      std::swap(last->name, name);
      std::swap(last->value, value);
   }
   void XMLDocument::addTextContent(UInt32 line, std::string& textContent) {
      if (this->stripWhitespace)
         cobb::trim(textContent);
      if (!textContent.size())
         return;
      this->tokens.emplace_back(line, kXMLToken_TextContent);
      auto last = this->tokens.rbegin();
      std::swap(last->value, textContent);
   }

   bool parseXML(XMLDocument& doc, const char* data, UInt32 size) {
      enum State {
         kState_InContent     = 0,
         kState_InOpenBracket = 1,
         kState_InOpenTag     = 2,
         kState_InCloseTag    = 3,
         kState_InAttribute   = 4,
         kState_InComment     = 5,
         kState_SelfClosing   = 6,
      };
      bool   isCDATA = false;
      State  state = kState_InContent;
      UInt32 stateChangeLine = 0;
      char   currentQuote     = '\0';
      UInt32 currentQuoteLine = 0;
      std::string name;
      std::string value;
      std::string lastTagName; // used for self-closing tags
      bool hasSpaces = false;
      //
      UInt32 lineNumber = 0;
      UInt32 colNumber  = 0;
      for (UInt32 i = 0; i < size; ++i) {
         char b = i > 0 ? data[i - 1] : '\0';
         char c = data[i];
         char d = i + 1 < size ? data[i + 1] : '\0';
         char e = i + 2 < size ? data[i + 2] : '\0';
         char f = i + 3 < size ? data[i + 3] : '\0';
         if (c == '\n') {
            lineNumber++;
            colNumber = 0;
         } else
            colNumber++;
         //
         if (i == 0) { // XML declaration; spec heavily implies it must be at the start of the file
            if (strncmp(data, "<?xml", 4) == 0) {
               auto at = strstr(data, "?>");
               if (!at) {
                  _MESSAGE("XML parse error on line %d: The XML declaration is unterminated.", lineNumber);
                  return false;
               }
               i = (at - data) + 2 - 1; // plus length of "?>", minus 1 since continue skips a char
               continue;
            }
         }
         //
         if (state != kState_InComment) {
            //
            // Handle CDATA.
            //
            if (!isCDATA) {
               if (i + 9 < size && strncmp(data + i, "<![CDATA[", 9) == 0) {
                  isCDATA = true;
                  i += 8; // continue skips one more char
                  continue;
               }
            } else {
               if (c == ']' && d == ']' && e == '>') {
                  isCDATA = false;
                  i += 2; // continue skips one more char
                  continue;
               }
               switch (state) {
                  case kState_InContent:
                     value += c;
                     break;
                  case kState_InOpenBracket:
                     name += c;
                     break;
                  case kState_InOpenTag:
                     name += c;
                     break;
                  case kState_InAttribute:
                     if (!currentQuote) {
                        _MESSAGE("XML parse error on line %d column %d: expected attribute value opening quote; got CDATA instead.", lineNumber, colNumber, c);
                        return false;
                     }
                     value += c;
                     break;
                  case kState_SelfClosing:
                     break;
                  case kState_InCloseTag:
                     name += c;
                     break;
               }
               continue;
            }
         }
         //
         // Handle non-CDATA:
         //
         if (state == kState_InContent) {
            if (c == '<') {
               name = "";
               lastTagName = "";
               state = kState_InOpenBracket;
               stateChangeLine = lineNumber;
               if (value.size()) {
                  doc.addTextContent(lineNumber, value);
                  value = "";
               }
               continue;
            }
            if (c == '&') {
               UInt32 target = _handleXMLEntity(doc, data + i, value, lineNumber, colNumber);
               if (target == 0)
                  return false;
               i += target - 1; // continue skips 1 char
               continue;
            }
            value += c;
            continue;
         }
         if (state == kState_InOpenBracket) {
            if (strchr("<&", c)) {
               _MESSAGE("XML parse error on line %d column %d: char '%c' is not valid after an opening angle bracket.", lineNumber, colNumber, c);
               return false;
            }
            if (!name.size()) {
               if (c == '/') {
                  state = kState_InCloseTag;
                  stateChangeLine = lineNumber;
                  continue;
               }
               if (b == '<' && c == '!' && d == '-' && e == '-') { // test for comments
                  state = kState_InComment;
                  stateChangeLine = lineNumber;
                  name  = "";
                  value = "";
                  i += 2; // continue will skip one more char
                  continue;
               }
               if (strchr(">=", c)) {
                  _MESSAGE("XML parse error on line %d column %d: opening tag with no tagname.", lineNumber, colNumber);
                  return false;
               }
               if (strchr("'\"&", c)) {
                  _MESSAGE("XML parse error on line %d column %d: unexpected '%c'.", lineNumber, colNumber, c);
                  return false;
               }
               if (c == '?') {
                  if (strncmp(data + i - 1, "<?xml", 5) == 0) {
                     _MESSAGE("XML parse error on line %d column %d: apparent XML declaration not at the very start of the file. There can't be anything before the declaration -- not even comments or whitespace.", lineNumber, colNumber);
                     return false;
                  }
                  _MESSAGE("XML parse error on line %d column %d: unexpected '?'.", lineNumber, colNumber);
                  return false;
               }
               if (!isspace(c))
                  name += c;
            } else {
               if (isspace(c)) {
                  lastTagName = name;
                  doc.addElementStart(lineNumber, name);
                  state = kState_InOpenTag;
                  stateChangeLine = lineNumber;
                  name  = "";
                  value = "";
                  continue;
               }
               if (c == '/') {
                  lastTagName = name;
                  state = kState_SelfClosing;
                  stateChangeLine = lineNumber;
                  continue;
               }
               if (c == '>') {
                  doc.addElementStart(lineNumber, name);
                  value = "";
                  state = kState_InContent;
                  stateChangeLine = lineNumber;
                  continue;
               }
               if (c == '=') {
                  _MESSAGE("XML parse error on line %d column %d: '=' glyph after a tag name.", lineNumber, colNumber);
                  return false;
               }
               if (c == '&') {
                  _MESSAGE("XML parse error on line %d column %d: '&' glyph not permitted here.", lineNumber, colNumber);
                  return false;
               }
               name += c;
            }
         }
         if (state == kState_InComment) {
            if (c == '-' && d == '-') {
               if (e == '>') {
                  state = kState_InContent;
                  stateChangeLine = lineNumber;
                  i += 2; // continue will skip one more char
                  continue;
               }
               _MESSAGE("XML parse error on line %d column %d: for compatibility with SGML, XML does not allow the string \"--\" in comments.", lineNumber, colNumber);
               return false;
            }
         }
         if (state == kState_InOpenTag) {
            if (isspace(c))
               continue;
            if (!name.size()) {
               if (strchr("<=&", c)) {
                  _MESSAGE("XML parse error on line %d column %d: expected attribute name or end of opening tag; got '%c'.", lineNumber, colNumber, c);
                  return false;
               }
               if (c == '/') {
                  state = kState_SelfClosing;
                  stateChangeLine = lineNumber;
                  continue;
               }
               if (c == '>') {
                  state = kState_InContent;
                  stateChangeLine = lineNumber;
                  value = "";
                  continue;
               }
            } else {
               if (c == '=') {
                  state = kState_InAttribute;
                  stateChangeLine = lineNumber;
                  value = "";
                  continue;
               }
               if (strchr("<>'\"/&", c)) {
                  _MESSAGE("XML parse error on line %d column %d: expected '=' glyph in attribute; got '%c' instead.", lineNumber, colNumber, c);
                  return false;
               }
            }
            name += c;
         }
         if (state == kState_InAttribute) {
            if (!currentQuote) {
               if (isspace(c))
                  continue;
               if (c == '"' || c == '\'') {
                  currentQuote = c;
                  currentQuoteLine = lineNumber;
                  continue;
               }
               _MESSAGE("XML parse error on line %d column %d: expected attribute value opening quote; got '%c' instead.", lineNumber, colNumber, c);
               return false;
            }
            if (c == currentQuote) {
               doc.addElementAttribute(lineNumber, name, value);
               state = kState_InOpenTag;
               stateChangeLine = lineNumber;
               currentQuote = '\0';
               continue;
            }
            if (c == '&') {
               UInt32 target = _handleXMLEntity(doc, data + i, value, lineNumber, colNumber);
               if (target == 0)
                  return false;
               i += target - 1; // continue skips 1 char
               continue;
            }
            value += c;
         }
         if (state == kState_SelfClosing) {
            if (isspace(c))
               continue;
            if (c == '>') {
               if (!lastTagName.size()) {
                  _MESSAGE("XML parse error on line %d column %d: unable to remember name of self-closing tag.", lineNumber, colNumber);
                  return false;
               }
               doc.addElementEnd(lineNumber, lastTagName);
               lastTagName = "";
               value = "";
               state = kState_InContent;
               stateChangeLine = lineNumber;
               continue;
            }
            _MESSAGE("XML parse error on line %d column %d: expected end of self-closing tag ('<'); got '%c'.", lineNumber, colNumber, c);
            return false;
         }
         if (state == kState_InCloseTag) {
            if (!name.size()) {
               if (isspace(c))
                  continue;
               if (c == '>') {
                  _MESSAGE("XML parse error on line %d column %d: closing tag with no tagname.", lineNumber, colNumber);
                  return false;
               }
               if (strchr("/='\"<&", c)) {
                  _MESSAGE("XML parse error on line %d column %d: expected a closing tagname; got '%c'.", lineNumber, colNumber, c);
                  return false;
               }
               name += c;
               hasSpaces = false;
            } else {
               if (isspace(c)) {
                  hasSpaces = true;
                  continue;
               }
               if (hasSpaces) {
                  _MESSAGE("XML parse error on line %d column %d: unknown content after a closing tag's tagname, beginning with '%c'.", lineNumber, colNumber, c);
                  return false;
               }
               if (strchr("/='\"<&", c)) {
                  _MESSAGE("XML parse error on line %d column %d: unexpected char '%c' found in a closing tag.", lineNumber, colNumber, c);
                  return false;
               }
               if (c == '>') {
                  doc.addElementEnd(lineNumber, name);
                  value = "";
                  state = kState_InContent;
                  stateChangeLine = lineNumber;
                  continue;
               }
               name += c;
            }
         }
      }
      if (isCDATA) {
         _MESSAGE("XML parse error: unterminated CDATA.");
         return false;
      }
      if (currentQuote) {
         _MESSAGE("XML parse error: runaway attribute-value with opening quotation mark '%c', beginning on line %d, reached the end of the document.", currentQuote, currentQuoteLine);
         return false;
      }
      if (state != kState_InContent) {
         switch (state) {
            case kState_InAttribute:
               _MESSAGE("XML parse error: runaway attribute reached the end of the document. Token began near line %d.", stateChangeLine);
               break;
            case kState_InComment:
               _MESSAGE("XML parse error: runaway comment reached the end of the document. Token began near line %d.", stateChangeLine);
               break;
            case kState_InCloseTag:
               if (name.size())
                  _MESSAGE("XML parse error: runaway close tag for tagname \"%s\" reached the end of the document. Token began near line %d.", name.c_str(), stateChangeLine);
               else
                  _MESSAGE("XML parse error: runaway close tag with no name reached the end of the document. Token began near line %d.", stateChangeLine);
               break;
            case kState_InOpenBracket:
               if (name.size())
                  _MESSAGE("XML parse error: runaway open-angle-bracket reached the end of the document; tagname may have been \"%s\". Token began near line %d.", name.c_str(), stateChangeLine);
               else
                  _MESSAGE("XML parse error: runaway open-angle-bracket reached the end of the document. Token began near line %d.", stateChangeLine);
               break;
            case kState_InOpenTag:
               if (name.size())
                  _MESSAGE("XML parse error: runaway open tag for tagname \"%s\" reached the end of the document. Token began near line %d.", name.c_str(), stateChangeLine);
               else
                  _MESSAGE("XML parse error: runaway open tag with no name reached the end of the document. Token began near line %d.", stateChangeLine);
               break;
            case kState_SelfClosing:
               if (lastTagName.size())
                  _MESSAGE("XML parse error: runaway self-closing tag for tagname \"%s\" reached the end of the document. Token began near line %d.", lastTagName.c_str(), stateChangeLine);
               else
                  _MESSAGE("XML parse error: runaway self-closing tag (unidentifiable) reached the end of the document. Token began near line %d.", stateChangeLine);
               break;
         }
         return false;
      }
      {  // Verify proper nesting.
         std::vector<XMLToken*> hierarchy;
         for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
            auto& token = *it;
            switch (token.code) {
               case kXMLToken_ElementOpen:
                  hierarchy.push_back(&token);
                  break;
               case kXMLToken_ElementClose:
                  if (!hierarchy.size()) {
                     _MESSAGE("XML parse error on line %d: close tag for \"%s\" found outside of any element.", token.line, token.name.c_str());
                     return false;
                  }
                  auto& last = (*hierarchy.rbegin())->name;
                  if (token.name != last) {
                     _MESSAGE("XML parse error on line %d: mismatched tags: expected close tag for \"%s\" and got close tag for \"%s\" instead.", token.line, last.c_str(), token.name.c_str());
                     return false;
                  }
                  hierarchy.pop_back();
                  break;
            }
         }
         auto size = hierarchy.size();
         if (size) {
            auto first = *hierarchy.begin();
            auto last  = *hierarchy.rbegin();
            if (size > 1)
               _MESSAGE("XML parse error: reached the end of the document with %d unclosed tags; the first was \"%s\" near line %d, and the last was \"%s\" near line %d.",
                  size,
                  first->name.c_str(),
                  first->line,
                  last->name.c_str(),
                  last->line
               );
            else
               _MESSAGE("XML parse error: reached the end of the document with an unclosed tag named \"%s\" near line %d.", first->name.c_str(), first->line);
            return false;
         }
      }
      return true;
   };
};