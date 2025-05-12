#pragma once
#include <string>
#include <log4cpp/Category.hh>
#include "config.hpp"
#include "semver.hpp"

namespace gtnh2Packwiz {
  class configFile {
    protected:
      struct configStruct {
        std::string repo;
        semVer version;
        std::string cfApiKey;
        bool generateRemoteHashes;
        std::string outPath;
      };
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".configFile");
      configStruct config;
    public:
      configFile(std::string filePath);
      const configStruct getConfig() const;
  };
}