#include "progress.hpp"

long double progress::get(int precision) {
    return roundToPrecision(value, precision);
}