#include "percentage.hpp"

int percentage::getWhole() {
    return static_cast<int>(roundToPrecision(value, 0));
}