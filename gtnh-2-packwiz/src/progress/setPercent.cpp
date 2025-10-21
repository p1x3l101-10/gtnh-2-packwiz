#include "progress.hpp"

void progress::setPercent(float percent) {
    prevValue = value;
    value = value + (percent * onePercent);
}