#include "gtnh2Packwiz/pack.hpp"
#include <filesystem>

std::filesystem::path gtnh2Packwiz::pack::getFile(std::string &fileName) {
  std::string processedName = "/" + fileName;
  std::filesystem::path file = outdir.string() + processedName;
  return file;
}