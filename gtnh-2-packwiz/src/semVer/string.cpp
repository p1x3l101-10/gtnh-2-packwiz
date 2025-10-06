#include <sstream>
#include "semver.hpp"

const std::string semVer::string() const {
    std::ostringstream out;
    out << version.major << "." << version.minor << "." << version.patch;
    if (version.usingPrerelease) {
        out << "-" << version.prerelease;
    }
    return out.str();
}