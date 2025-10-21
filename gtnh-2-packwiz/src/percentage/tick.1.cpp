#include "percentage.hpp"

void percentage::tick(float percent) {
    prevValue = value;
    value = value + (percent * onePercent);
}