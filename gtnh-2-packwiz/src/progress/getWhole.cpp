#include "progress.hpp"

int progress::getWhole() {
    return static_cast<int>(get(0));
}