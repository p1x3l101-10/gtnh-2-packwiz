#include "percentage.hpp"

long double percentage::get(int precision) {
    return roundToPrecision(value, precision);
}