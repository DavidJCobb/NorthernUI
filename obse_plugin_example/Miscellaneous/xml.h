#pragma once
#include <string>
#include <vector>

namespace cobb {
   enum XMLTokenCode : UInt32 {
      kXMLToken_Undefined    = 0,
      //kXMLToken_TagStart     = 1,
      //kXMLToken_TagEnd       = 2,
      kXMLToken_Attribute    = 3,
      kXMLToken_ElementOpen  = 4,
      kXMLToken_ElementClose = 5,
      kXMLToken_TextContent  = 6,
   };
   struct XMLToken {
      XMLToken(UInt32 line, XMLTokenCode a) : line(line), code(a) {};
      XMLToken(UInt32 line, XMLTokenCode a, float b, const char* c, const char* d) : line(line), code(a), number(b), name(c), value(d) {};

      UInt32 line = 0;
      XMLTokenCode code = kXMLToken_Undefined;
      float  number = 0.0F;
      std::string name;
      std::string value;
   };
   struct XMLDocument {
      struct CustomEntity {
         const char* entity; // "&name;"
         const char* substitution;

         CustomEntity(const char* a, const char* b) : entity(a), substitution(b) {};
      };

      std::vector<XMLToken>     tokens;
      std::vector<CustomEntity> entities;
      bool stripWhitespace = false;

      void defineEntity(const char* entity, const char* substitution);

      //
      // meetsStrictRequirements
      //
      //    Returns true if the document has only one top-level node i.e. only 
      //    one root. May add additional tests to this in the future.
      //
      bool meetsStrictRequirements() const; // TODO: should probably just be built into the parser

      //
      // Used by the parser:
      //
      void addElementStart(UInt32 line, std::string& tagName); // uses std::swap to take ownership of the string
      void addElementEnd(UInt32 line, std::string& tagName); // uses std::swap to take ownership of the string
      void addElementAttribute(UInt32 line, std::string& name, std::string& value); // uses std::swap to take ownership of the strings
      void addTextContent(UInt32 line, std::string& textContent); // uses std::swap to take ownership of the string
   };

   //
   // TODO:
   //
   //  - Refactor slightly so that we just pass in the const char* 
   //    pointer and assume it's null-terminated; saves us the 
   //    overhead of having the caller do strlen.
   //
   bool parseXML(XMLDocument& doc, const char* data, UInt32 size);
};