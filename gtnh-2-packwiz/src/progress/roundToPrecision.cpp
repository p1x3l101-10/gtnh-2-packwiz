#include <cmath>
#include "progress.hpp"

long double progress::roundToPrecision(long double value, int decimalPlaces) {
    long double multiplier = std::pow(10.0, decimalPlaces);
    return std::round(value * multiplier) / multiplier;
}