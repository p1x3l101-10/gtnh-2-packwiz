#include "semver.hpp"

semVer::semVer(int major, int minor, int patch, std::string prerelease) {
  version.major = major;
  version.minor = minor;
  version.patch = patch;
  if (prerelease.empty()) {
    version.prerelease = "";
    version.usingPrerelease = false;
  } else {
    version.usingPrerelease = true;
  }
}