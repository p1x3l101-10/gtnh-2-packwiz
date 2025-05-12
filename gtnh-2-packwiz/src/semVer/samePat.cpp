#include "semver.hpp"

bool semVer::samePat(semVer other) {
  return (sameMin(other) && (other.raw().minor == version.minor));
}