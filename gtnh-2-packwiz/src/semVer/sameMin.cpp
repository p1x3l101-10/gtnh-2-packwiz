#include "semver.hpp"

bool semVer::sameMin(semVer other) {
    return (sameMaj(other) && (other.raw().minor == version.minor));
}