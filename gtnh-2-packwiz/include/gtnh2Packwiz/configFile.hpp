#pragma once
#include <log4cpp/Category.hh>
#include <string>
#include "config.hpp"

namespace gtnh2Packwiz {
  class configFile {
    protected:
      struct configStruct {
          std::string repo;
          std::string configRepo;
          std::string cfApiKey;
          bool generateRemoteHashes;
          std::string outPath;
      };
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".configFile");
      configStruct config;
    public:
      configFile(std::string filePath);
      const configStruct getConfig() const;
  };
} // namespace gtnh2Packwiz