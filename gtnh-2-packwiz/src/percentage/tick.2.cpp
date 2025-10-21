#include "percentage.hpp"

void percentage::tick(long double amount) {
    prevValue = value;
    value = value + amount;
}