#include "gtnh2Packwiz/pack.hpp"
#include <filesystem>

namespace fs = std::filesystem;

void gtnh2Packwiz::pack::deleteFile(std::string fileName) {
  fs::path file = getFile(fileName);
  if (fs::exists(file)) {
    fs::remove(file);
  }
}