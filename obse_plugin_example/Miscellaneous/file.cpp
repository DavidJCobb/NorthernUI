#include "file.h"

namespace cobb {
   void ifstream_to_string(std::string& target, std::ifstream& stream) {
      char buffer[512];
      do {
         stream.get(buffer, sizeof(buffer));
         target += buffer;
         memset(buffer, 0, stream.gcount());
         if (!stream) {
            //
            // ... error ...
            //
            return;
         }
      } while (!stream.eof());
   }
   void ifstream_to_string(std::string& target, std::ifstream& stream, UInt32 size) {
      target.resize(size + 1);
      stream.read(&target[0], size);
      target[size + 1] = '\0';
   }
};