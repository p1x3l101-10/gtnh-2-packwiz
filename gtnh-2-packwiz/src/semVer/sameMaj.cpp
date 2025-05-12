#include "semver.hpp"

bool semVer::sameMaj(semVer other) {
  return (other.raw().major == version.major);
}