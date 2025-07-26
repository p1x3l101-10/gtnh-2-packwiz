#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include "gtnh2Packwiz/configFile.hpp"
#include "semver.hpp"

namespace gtnh2Packwiz {
  class pack {
    protected:
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".configFile");
      const semVer packVersion;
      const std::shared_ptr<gtnh2Packwiz::configFile> config;
      const std::filesystem::path outdir;
      std::ofstream writeToOut(std::string& fileName);
      std::filesystem::path getFile(std::string& fileName);
      void deleteFile(std::string fileName);
      void extractZip(std::filesystem::path zip, std::string outDir);
    public:
      void build();
      pack(std::string version, std::shared_ptr<gtnh2Packwiz::configFile> config);
  };
} // namespace gtnh2Packwiz