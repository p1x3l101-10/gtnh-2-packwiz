#include "progress.hpp"

void progress::setProgress(int progress) {
    prevValue = value;
    value = (static_cast<long double>(capacity) / progress);
}