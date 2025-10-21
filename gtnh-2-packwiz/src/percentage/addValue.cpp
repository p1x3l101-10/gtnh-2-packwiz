#include "percentage.hpp"

void percentage::addValue(long double amount) {
    prevValue = value;
    value = value + amount;
}