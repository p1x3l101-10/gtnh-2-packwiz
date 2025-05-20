#include "gtnh2Packwiz/filecache.hpp"
#include <filesystem>

std::shared_future<std::filesystem::path> gtnh2Packwiz::filecache::getFile() const {
  return file;
}