#include "UIPrefs.h"

enum TokenCode : UInt32 {
   kToken_Undefined    = 0,
   //kToken_TagStart     = 1,
   //kToken_TagEnd       = 2,
   kToken_Attribute    = 3,
   kToken_ElementOpen  = 4,
   kToken_ElementClose = 5,
   kToken_TextContent  = 6,
};
struct Token {
   Token(UInt32 line, TokenCode a) : line(line), code(a) {};
   Token(UInt32 line, TokenCode a, float b, const char* c, const char* d) : line(line), code(a), number(b), name(c), value(d) {};

   UInt32 line = 0;
   TokenCode code = kToken_Undefined;
   float  number = 0.0F;
   std::string name;
   std::string value;
};
struct Document {
   std::vector<Token> tokens;

   void addElementStart(UInt32 line, std::string& tagName) {
      this->tokens.emplace_back(line, kToken_ElementOpen);
      auto last = this->tokens.rbegin();
      std::swap(last->name, tagName);
   }
   void addElementEnd(UInt32 line, std::string& tagName) {
      this->tokens.emplace_back(line, kToken_ElementClose);
      auto last = this->tokens.rbegin();
      std::swap(last->name, tagName);
   }
   void addElementAttribute(UInt32 line, std::string& name, std::string& value) {
      this->tokens.emplace_back(line, kToken_Attribute);
      auto last = this->tokens.rbegin();
      std::swap(last->name,  name);
      std::swap(last->value, value);
   }
   void addTextContent(UInt32 line, std::string& textContent) {
      this->tokens.emplace_back(line, kToken_TextContent);
      auto last = this->tokens.rbegin();
      std::swap(last->value, textContent);
   }
};

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

UInt32 _handleXMLEntity(const char* ampersand, std::string& writeTo, UInt32 lineNumber, UInt32 colNumber) {
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
   UInt32 whichEntity = 0;
   bool   match = false;
   UInt32 k;
   do {
      const char* entity = entities[whichEntity];
      k = 0;
      char strChar;
      do {
         strChar = ampersand[k];
         if (entity[k] == '\0')
            match = true;
         if (entity[k] != strChar)
            break;
         k++;
      } while (strChar);
      if (match)
         break;
   } while (++whichEntity < std::extent<decltype(entities)>::value);
   if (match) {
      writeTo += results[whichEntity];
      return k - 1; // - 1 to omit the null terminator in the entity name string
   }
   _MESSAGE("XML parse error on line %d column %d: found a '&' glyph that isn't a valid entity.", lineNumber, colNumber);
   return 0;
}

bool ParseBuffer(Document& doc, const char* data, UInt32 size) {
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
            UInt32 target = _handleXMLEntity(data + i, value, lineNumber, colNumber);
            if (target == 0)
               return false;
            i += target;
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
            if (strchr(">='\'&", c)) {
               _MESSAGE("XML parse error on line %d column %d: opening tag with no tagname.", lineNumber, colNumber);
               return false;
            }
            if (c == '?') {
               //
               // TODO: could be an XML declaration
               //
               continue;
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
            UInt32 target = _handleXMLEntity(data + i, value, lineNumber, colNumber);
            if (target == 0)
               return false;
            i += target;
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
      std::vector<Token*> hierarchy;
      for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
         auto& token = *it;
         switch (token.code) {
            case kToken_ElementOpen:
               hierarchy.push_back(&token);
               break;
            case kToken_ElementClose:
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

void _RunPrefXMLParseTest() {
   const char* code[] = {
      "<root>\n   Foo\n</root>",
      "<root attr='value'>\n   Foo\n</root>",
      "<root attr='value'>\n   <foo />\n   <bar></bar>\n</root>",
      "<root>&amp;</root>",
      "<root>&#45;</root>",
      "<root>&lt;fake-nested&gt;</root>",
      "<root attr='&lt;value&gt;'></root>",
      "<!-- comment test -->",
      "<!-- <root> -->",
      "<!-- & -->",
      "<!---->",
      "<!--<![CDATA[ test ]]>-->",
      "<root><![CDATA[<nested>]]></root>",
      "<<![CDATA[root]]> attr='value'>foo</root>",
      "<root <![CDATA[attr]]>='value'>foo</root>",
      "<root attr='<![CDATA[value]]>'>foo</root>",
      "<root attr='value'>foo</<![CDATA[root]]>>",
      "<<![CDATA[ro!ot]]> attr='value'>foo</<![CDATA[ro!ot]]>>",
      "<root attr='value\">\n   <foo />\n   <bar></bar>\n</root>", // mismatched quotes on the attribute value
      "<root", // unterminated opening tag
      "<", // lone angle bracket
      "<root></root", // unterminated closing tag
      "<root>\n   <nested>\n</root>", // unterminated inner element
      "<root>\n<nested/>", // mismatched closing tags
      "<root>&</root>", // lone ampersand
      "<root>&amp</root>", // unterminated entity
      "<root>&#;</root>", // character entity with no character code
      "<root>&#x;</root>", // character entity with no character code (hexadecimal)
      "<root>&#x00;</root>", // character entity using a null character code
      "<root>&fake;</root>", // unrecognized entity
      "<!-- comment test", // unterminated comment
      "<!-->", // incorrect "compact" comment
      "<!-- -- -->", // "--" is not allowed in comments
   };
   _MESSAGE("== RUNNING PREF PARSE TESTS ==");
   for (UInt32 i = 0; i < std::extent<decltype(code)>::value; i++) {
      _MESSAGE("===== XML PARSE TEST %2d =====", i);
      Document doc;
      if (ParseBuffer(doc, code[i], strlen(code[i]))) {
         _MESSAGE("DOCUMENT PARSING COMPLETE. REPLICATING...");
         bool unclosed = false;
         for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
            auto& token = *it;
            switch (token.code) {
               case kToken_TextContent:
                  if (unclosed) {
                     _MESSAGE(">");
                     unclosed = false;
                  }
                  _MESSAGE(token.value.c_str());
                  break;
               case kToken_ElementOpen:
                  unclosed = true;
                  _MESSAGE("<%s", token.name.c_str());
                  break;
               case kToken_Attribute:
                  _MESSAGE(" %s=\"%s\"", token.name.c_str(), token.value.c_str());
                  break;
               case kToken_ElementClose:
                  if (unclosed) {
                     _MESSAGE(">");
                     unclosed = false;
                  }
                  _MESSAGE("</%s>", token.name.c_str());
                  break;
            }
         }
         _MESSAGE("END OF DOCUMENT");
      }
      _MESSAGE("\n");
   }
   _MESSAGE("== ALL PARSE TESTS COMPLETE ==");
}