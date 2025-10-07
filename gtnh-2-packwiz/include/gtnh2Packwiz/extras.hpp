#pragma once

#include <filesystem>
#include <string>
#include "config.hpp"

namespace gtnh2Packwiz::extras {
    void downloadFile(std::string url, std::filesystem::path destination);
    void extractZip(std::filesystem::path zip, std::filesystem::path outDir);
    template <size_t BASE = 1024, std::enable_if_t<BASE == 1000 || BASE == 1024, int> = 0>
        const std::string humanReadableBytes(uintmax_t size, unsigned precision = 0);
} // namespace gtnh2Packwiz::extras