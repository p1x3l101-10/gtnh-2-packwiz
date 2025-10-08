#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include "config.hpp"

namespace gtnh2Packwiz::extras {
    void downloadFile(std::string url, std::filesystem::path destination);
    void extractZip(std::filesystem::path zip, std::filesystem::path outDir);
    const std::string humanReadableBytes(uint64_t size, unsigned precision = 0);
} // namespace gtnh2Packwiz::extras