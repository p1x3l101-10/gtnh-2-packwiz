#include "percentage.hpp"

void percentage::setPercent(float percent) {
    prevValue = value;
    value = value + (percent * onePercent);
}