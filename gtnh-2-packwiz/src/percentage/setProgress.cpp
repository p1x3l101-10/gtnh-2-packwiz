#include "percentage.hpp"

void percentage::setProgress(int progress) {
    prevValue = value;
    value = (static_cast<long double>(capacity) / progress);
}