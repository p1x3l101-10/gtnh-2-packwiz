#include "semver.hpp"

bool semVer::samePre(semVer other) {
  bool iden = samePre(other);
  semVerStruct otherRaw = other.raw();
  if (version.usingPrerelease && otherRaw.usingPrerelease) {
    return (version.prerelease == otherRaw.prerelease);
  } else if ((! version.usingPrerelease) && (! otherRaw.usingPrerelease)) {
    return true;
  } else {
    return false;
  }
}