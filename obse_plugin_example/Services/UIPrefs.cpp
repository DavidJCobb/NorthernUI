#include "UIPrefs.h"

enum {
   //kToken_TagStart     = 1,
   //kToken_TagEnd       = 2,
   kToken_Attribute    = 3,
   kToken_ElementOpen  = 4,
   kToken_ElementClose = 5,
   kToken_TextContent  = 6,
};
struct Token {
   Token(UInt32 a) : code(a) {};
   Token(UInt32 a, float b, const char* c, const char* d) : code(a), number(b), name(c), value(d) {};

   UInt32 code   = 0;
   float  number = 0.0F;
   std::string name;
   std::string value;
};
struct Document {
   std::vector<Token> tokens;

   void addElementStart(std::string& tagName) {
      this->tokens.emplace_back(kToken_ElementOpen);
      auto last = this->tokens.rbegin();
      std::swap(last->name, tagName);
   }
   void addElementEnd(std::string& tagName) {
      this->tokens.emplace_back(kToken_ElementClose);
      auto last = this->tokens.rbegin();
      std::swap(last->name, tagName);
   }
   void addElementAttribute(std::string& name, std::string& value) {
      this->tokens.emplace_back(kToken_Attribute);
      auto last = this->tokens.rbegin();
      std::swap(last->name,  name);
      std::swap(last->value, value);
   }
   void addTextContent(std::string& textContent) {
      this->tokens.emplace_back(kToken_TextContent);
      auto last = this->tokens.rbegin();
      std::swap(last->value, textContent);
   }
};

void ParseBuffer(Document& doc, const char* data, UInt32 size) {
   enum State {
      kState_InContent     = 0,
      kState_InOpenBracket = 1,
      kState_InOpenTag     = 2,
      kState_InCloseTag    = 3,
      kState_InAttribute   = 4,
      kState_InComment     = 5,
      kState_SelfClosing   = 6,
   };
   State  state = kState_InContent;
   char   currentQuote     = '\0';
   UInt32 currentQuoteLine = 0;
   std::string name;
   std::string value;
   std::string lastTagName; // used for self-closing tags
   bool hasSpaces = false;
   //
   UInt32   lineNumber = 0;
   for (UInt32 i = 0; i < size; ++i) {
      char b = i > 0 ? data[i - 1] : '\0';
      char c = data[i];
      char d = i + 1 < size ? data[i + 1] : '\0';
      char e = i + 2 < size ? data[i + 2] : '\0';
      char f = i + 3 < size ? data[i + 3] : '\0';
      if (c == '\n')
         lineNumber++;
      if (state == kState_InContent) {
         if (c == '<') {
            name = "";
            lastTagName = "";
            state = kState_InOpenBracket;
            if (value.size()) {
               doc.addTextContent(value);
               value = "";
            }
            continue;
         }
         value += c;
         continue;
      }
      if (state == kState_InOpenBracket) {
         if (strchr("<", c)) {
            _MESSAGE("XML parse error on line %d: char '%c' is not valid after an opening angle bracket.", lineNumber, c);
            return;
         }
         if (!name.size()) {
            if (c == '/') {
               state = kState_InCloseTag;
               continue;
            }
            if (b == '<' && c == '!' && d == '-' && e == '-') { // test for comments
               state = kState_InComment;
               name  = "";
               value = "";
               i += 1; // continue will skip one more char
               continue;
            }
            if (strchr(">='\'", c)) {
               _MESSAGE("XML parse error on line %d: opening tag with no tagname.", lineNumber);
               return;
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
               doc.addElementStart(name);
               state = kState_InOpenTag;
               name  = "";
               value = "";
               continue;
            }
            if (c == '/') {
               lastTagName = name;
               state = kState_SelfClosing;
               continue;
            }
            if (c == '>') {
               doc.addElementStart(name);
               value = "";
               state = kState_InContent;
               continue;
            }
            if (c == '=') {
               _MESSAGE("XML parse error on line %d: '=' glyph after a tag name.", lineNumber);
               return;
            }
            name += c;
         }
      }
      if (state == kState_InComment) {
         if (c == '-' && d == '-' && e == '>') {
            state = kState_InContent;
            i += 1; // continue will skip one more char
            continue;
         }
      }
      if (state == kState_InOpenTag) {
         if (isspace(c))
            continue;
         if (!name.size()) {
            if (c == '=') {
               _MESSAGE("XML parse error on line %d: expected attribute name or end of opening tag; got '%c'.", lineNumber, c);
               return;
            }
            if (c == '/') {
               state = kState_SelfClosing;
               continue;
            }
            if (c == '>') {
               state = kState_InContent;
               value = "";
               continue;
            }
         } else {
            if (c == '=') {
               state = kState_InAttribute;
               value = "";
               continue;
            }
            if (strchr("<>'\"/", c)) {
               _MESSAGE("XML parse error on line %d: expected '=' glyph in attribute; got '%c' instead.", lineNumber, c);
               return;
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
            _MESSAGE("XML parse error on line %d: expected attribute value opening quote; got '%c' instead.", lineNumber, c);
            return;
         }
         if (c == currentQuote) {
            doc.addElementAttribute(name, value);
            state = kState_InOpenTag;
            currentQuote = '\0';
            continue;
         }
         value += c;
      }
      if (state == kState_SelfClosing) {
         if (isspace(c))
            continue;
         if (c == '>') {
            if (!lastTagName.size()) {
               _MESSAGE("XML parse error on line %d: unable to remember name of self-closing tag.", lineNumber);
               return;
            }
            doc.addElementEnd(lastTagName);
            lastTagName = "";
            value = "";
            state = kState_InContent;
            continue;
         }
         _MESSAGE("XML parse error on line %d: expected end of self-closing tag ('<'); got '%c'.", lineNumber, c);
         return;
      }
      if (state == kState_InCloseTag) {
         if (!name.size()) {
            if (isspace(c))
               continue;
            if (c == '>') {
               _MESSAGE("XML parse error on line %d: closing tag with no tagname.", lineNumber);
               return;
            }
            if (strchr("/='\"<", c)) {
               _MESSAGE("XML parse error on line %d: expected a closing tagname; got '%c'.", lineNumber, c);
               return;
            }
            name += c;
            hasSpaces = false;
         } else {
            if (isspace(c)) {
               hasSpaces = true;
               continue;
            }
            if (hasSpaces) {
               _MESSAGE("XML parse error on line %d: unknown content after a closing tag's tagname, beginning with '%c'.", lineNumber, c);
               return;
            }
            if (strchr("/='\"<", c)) {
               _MESSAGE("XML parse error on line %d: unexpected char '%c' found in a closing tag.", lineNumber, c);
               return;
            }
            if (c == '>') {
               doc.addElementEnd(name);
               value = "";
               state = kState_InContent;
               continue;
            }
            name += c;
         }
      }
   }
   if (currentQuote) {
      _MESSAGE("XML parse error: runaway attribute-value with opening quotation mark '%c', beginning on line %d, reached the end of the document.", currentQuote, currentQuoteLine);
      return;
   }
   if (state != kState_InContent) {
      //
      // TODO: keep track of the line number for the last state change so we can log these better
      //
      switch (state) {
         case kState_InAttribute:
            _MESSAGE("XML parse error: runaway attribute reached the end of the document.");
            break;
         case kState_InComment:
            _MESSAGE("XML parse error: runaway comment reached the end of the document.");
            break;
         case kState_InCloseTag:
            if (name.size())
               _MESSAGE("XML parse error: runaway close tag for tagname \"%s\" reached the end of the document.", name.c_str());
            else
               _MESSAGE("XML parse error: runaway close tag with no name reached the end of the document.");
            break;
         case kState_InOpenBracket:
            if (name.size())
               _MESSAGE("XML parse error: runaway open-angle-bracket reached the end of the document; tagname may have been \"%s\".", name.c_str());
            else
               _MESSAGE("XML parse error: runaway open-angle-bracket reached the end of the document.");
            break;
         case kState_InOpenTag:
            if (name.size())
               _MESSAGE("XML parse error: runaway open tag for tagname \"%s\" reached the end of the document.", name.c_str());
            else
               _MESSAGE("XML parse error: runaway open tag with no name reached the end of the document.");
            break;
         case kState_SelfClosing:
            if (lastTagName.size())
               _MESSAGE("XML parse error: runaway self-closing tag for tagname \"%s\" reached the end of the document.", lastTagName.c_str());
            else
               _MESSAGE("XML parse error: runaway self-closing tag (unidentifiable) reached the end of the document.");
            break;
      }
      return;
   }
   {  // Verify proper nesting.
      std::vector<std::string> hierarchy;
      for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
         auto& token = *it;
         switch (token.code) {
            case kToken_ElementOpen:
               hierarchy.push_back(token.name);
               break;
            case kToken_ElementClose:
               if (!hierarchy.size()) {
                  _MESSAGE("XML parse error: close tag for \"%s\" found outside of any element.", token.name.c_str());
                  return;
               }
               auto& last = *hierarchy.rbegin();
               if (token.name != last) {
                  _MESSAGE("XML parse error: mismatched tags: expected close tag for \"%s\" and got close tag for \"%s\" instead.", last.c_str(), token.name.c_str());
                  return;
               }
               break;
         }
      }
      auto size = hierarchy.size();
      if (size) {
         auto first = hierarchy.begin();
         auto last  = hierarchy.rbegin();
         if (size > 1)
            _MESSAGE("XML parse error: reached the end of the document with %d unclosed tags; the first was \"%s\" and the last was \"%s\".", size, first->c_str(), last->c_str());
         else
            _MESSAGE("XML parse error: reached the end of the document with an unclosed tag named \"%s\".", first->c_str());
         return;
      }
   }
   //
   // TODO: verify proper nesting and the like
   //
   //
   // ... done.
   //
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
};

void _RunPrefXMLParseTest() {
   const char* code[] = {
      "<root>\n   Foo\n</root>",
      "<root attr='value'>\n   Foo\n</root>",
      "<root attr='value'>\n   <foo />\n   <bar></bar>\n</root>",
      "<root attr='value\">\n   <foo />\n   <bar></bar>\n</root>",
      "<root",
      "<",
      "<root></root",
      "<root>\n   <nested>\n</root>",
      "<root>\n<nested/>",
   };
   _MESSAGE("== RUNNING PREF PARSE TESTS ==");
   for (UInt32 i = 0; i < std::extent<decltype(code)>::value; i++) {
      _MESSAGE("===== XML PARSE TEST %2d =====", i);
      Document doc;
      ParseBuffer(doc, code[i], strlen(code[i]));
      _MESSAGE("\n");
   }
   _MESSAGE("== ALL PARSE TESTS COMPLETE ==");
}