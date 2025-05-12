#include "semver.hpp"

const semVer::semVerStruct semVer::raw() const {
  return version;
}