#include "Types.h"
#include "Miscellaneous/strings.h"

namespace _dme {
   void Undifferentiated::PrepField(std::string& key, float*& outFloat, UInt32*& outFormID, std::string*& outString) {
      outFloat  = nullptr;
      outFormID = nullptr;
      outString = nullptr;
      //
      if (cobb::strieq(key, "mod"))
         outString = &this->name;
      else if (cobb::strieq(key, "or_mod"))
         outString = &this->name_alternate;
      else if (cobb::strieq(key, "x"))
         outFloat = &this->x;
      else if (cobb::strieq(key, "y"))
         outFloat = &this->y;
      else if (cobb::strieq(key, "w"))
         outFloat = &this->width;
      else if (cobb::strieq(key, "h"))
         outFloat = &this->height;
      else if (cobb::strieq(key, "nw_x"))
         outFloat = &this->xMin;
      else if (cobb::strieq(key, "nw_y"))
         outFloat = &this->yMin;
      else if (cobb::strieq(key, "se_x"))
         outFloat = &this->xMax;
      else if (cobb::strieq(key, "se_y"))
         outFloat = &this->yMax;
      else if (cobb::strieq(key, "zoom"))
         outFloat = &this->zoom;
      else if(cobb::strieq(key, "fid"))
         outFormID = &this->formID;
   };
   void Undifferentiated::SetField(std::string key, std::string lineRemainder) {
      std::string* outS  = nullptr;
      float*       outF  = nullptr;
      UInt32*      outID = nullptr;
      this->PrepField(key, outF, outID, outS);
      if (outS) {
         size_t b = lineRemainder.find_last_of('"');
         if (b == std::string::npos)
            return;
         size_t a = lineRemainder.find_last_of('"', b - 1);
         if (a == std::string::npos)
            return;
         outS->assign(lineRemainder.substr(a, b - a));
_MESSAGE("Stored key %s = %s", key.c_str(), outS->c_str());
      } else if (outID) {
         size_t a = lineRemainder.find_last_of('"');
         if (a == std::string::npos)
            return;
         a = lineRemainder.find_last_of('"', a - 1);
         if (a == std::string::npos)
            return;
         a++; // exclude first quote
         UInt32 formID = std::stoi(lineRemainder, &a, 16);
         *outID = formID;
_MESSAGE("Stored key %s = %08X", key.c_str(), formID);
      } else if (outF) {
         size_t a = lineRemainder.find_last_not_of(" \r\n\t");
         if (a == std::string::npos)
            return;
         a = lineRemainder.find_last_not_of(" \r\n\t", a - 1);
         if (a == std::string::npos)
            return;
         float v = std::stof(lineRemainder, &a);
         *outF = v;
_MESSAGE("Stored key %s = %f", key.c_str(), v);
      }
   };
};