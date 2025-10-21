#include "progress.hpp"

void progress::addValue(long double amount) {
    prevValue = value;
    value = value + amount;
}