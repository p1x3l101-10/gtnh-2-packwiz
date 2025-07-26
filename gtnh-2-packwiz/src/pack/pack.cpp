#include "gtnh2Packwiz/pack.hpp"

gtnh2Packwiz::pack::pack(std::string version, std::shared_ptr<gtnh2Packwiz::configFile> configPtr)
    : packVersion(version), config(configPtr) {}