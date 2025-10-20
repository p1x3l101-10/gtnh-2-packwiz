#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <nlohmann/json.hpp>
#include "config.hpp"

namespace gtnh2Packwiz::extras {
    struct expirationConditions {
        const bool enableExpiration;
        const bool keepOld; // Only in effect if expiration is disabled
        const std::chrono::duration<double> expiration;
        expirationConditions(bool enable, bool old)
        : enableExpiration(enable)
        , keepOld(old)
        , expiration(std::chrono::hours(12))
        {}
        expirationConditions(bool enable, bool old, std::chrono::duration<double> expiration)
        : enableExpiration(enable)
        , keepOld(old)
        , expiration(expiration)
        {}
        // This one is an unconditional redownload and only determines if to keep the old file
        expirationConditions(bool keepOld)
        : enableExpiration(false)
        , keepOld(keepOld)
        , expiration(std::chrono::hours(12))
        {}
    };
    void downloadFile(std::string url, std::filesystem::path destination, bool debugDownload = false, expirationConditions expirationConditions = { true, false, std::chrono::hours(12)});
    void githubSafeDlFile(std::string url, std::filesystem::path destination, bool debugDownload = false, expirationConditions expirationConditions = { true, false, std::chrono::hours(12)});
    void extractZip(std::filesystem::path zip, std::filesystem::path outDir, expirationConditions expirationConditions = { true, false, std::chrono::hours(8)});
    const std::string humanReadableBytes(uint64_t size, unsigned precision = 0);
    const std::string generatePWHash(std::filesystem::path file, std::string pwHashFormat);
    void callSignify(std::filesystem::path file, std::filesystem::path signature, std::filesystem::path key);
    nlohmann::json getModVersion(std::shared_ptr<nlohmann::json> gtnhAssets, std::string name, std::string version);
    std::string convertSidedness(std::string side);
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