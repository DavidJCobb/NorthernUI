#include "UIPrefs.h"
#include "Miscellaneous/xml.h"

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
      "<ui> <trait>&true;</trait> <trait> &false; </trait> </ui>",
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
      //
      cobb::XMLDocument doc;
      doc.stripWhitespace = true;
      doc.defineEntity("&true;",  "2");
      doc.defineEntity("&false;", "1");
      //
      if (cobb::parseXML(doc, code[i], strlen(code[i]))) {
         _MESSAGE("DOCUMENT PARSING COMPLETE. REPLICATING...");
         bool unclosed = false;
         for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
            auto& token = *it;
            switch (token.code) {
               case cobb::kXMLToken_TextContent:
                  if (unclosed) {
                     _MESSAGE(">");
                     unclosed = false;
                  }
                  _MESSAGE(token.value.c_str());
                  break;
               case cobb::kXMLToken_ElementOpen:
                  unclosed = true;
                  _MESSAGE("<%s", token.name.c_str());
                  break;
               case cobb::kXMLToken_Attribute:
                  _MESSAGE(" %s=\"%s\"", token.name.c_str(), token.value.c_str());
                  break;
               case cobb::kXMLToken_ElementClose:
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