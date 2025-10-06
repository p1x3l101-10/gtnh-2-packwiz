#pragma once

#include <filesystem>
#include <string>

namespace gtnh2Packwiz::extras {
  void downloadFile(std::string url, std::filesystem::path destination);
  void extractZip(std::filesystem::path zip, std::filesystem::path outDir);
} // namespace gtnh2Packwiz::extras