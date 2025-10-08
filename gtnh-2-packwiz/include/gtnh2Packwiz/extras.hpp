#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "config.hpp"

namespace gtnh2Packwiz::extras {
    void downloadFile(std::string url, std::filesystem::path destination);
    void extractZip(std::filesystem::path zip, std::filesystem::path outDir);
    const std::string humanReadableBytes(uint64_t size, unsigned precision = 0);
    // Why must templates be horrible like this???
    template <typename T>
        std::vector<T> setIntersection(std::vector<T> a, std::vector<T> b) {
            std::vector<T> result;
            // Sort lists
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            // Call the actual STL function i am wrapping
            std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.begin()));
            return result;
        };
} // namespace gtnh2Packwiz::extras