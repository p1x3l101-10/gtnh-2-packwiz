#include <cmath>
#include "percentage.hpp"

long double percentage::roundToPrecision(long double value, int decimalPlaces) {
    long double multiplier = std::pow(10.0, decimalPlaces);
    return std::round(value * multiplier) / multiplier;
}