#include "semver.hpp"
#include <regex>

semVer::semVer(std::string semverString) {
    std::regex semver_regex(R"(^(\d+)\.(\d+)\.(\d+)(?:-([\w\.-]+))?$)");
    std::smatch match;
    if (std::regex_match(semverString, match, semver_regex)) {
        version.major = std::stoi(match[1]);
        version.minor = std::stoi(match[2]);
        version.patch = std::stoi(match[3]);
        std::string prerelease = match[4].matched ? match[4].str() : "";
        if (!prerelease.empty()) {
            version.prerelease = prerelease;
            version.usingPrerelease = true;
        } else {
            version.usingPrerelease = false;
        }
    }
}