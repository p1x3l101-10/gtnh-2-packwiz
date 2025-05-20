#pragma once
#include <string>
#include "gtnh2Packwiz/configFile.hpp"

namespace gtnh2Packwiz {
  class pack {
    protected:
    private:
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".configFile");
      std::string packVersion;
      std::shared_ptr<gtnh2Packwiz::configFile> config;
    public:
      void build();
      pack(std::string version, std::shared_ptr<gtnh2Packwiz::configFile> config);
  };
}