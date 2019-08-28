#pragma once

namespace cobb {
   void ifstream_to_string(std::string& target, std::ifstream& stream);
   void ifstream_to_string(std::string& target, std::ifstream& stream, UInt32 size);
};