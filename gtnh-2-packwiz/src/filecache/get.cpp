#include "gtnh2Packwiz/filecache.hpp"
#include <fstream>

using nlohmann::json;

nlohmann::json gtnh2Packwiz::filecache::get() const {
  std::ifstream fstream(file.get());
  json data = json::parse(fstream);
  return data;
}