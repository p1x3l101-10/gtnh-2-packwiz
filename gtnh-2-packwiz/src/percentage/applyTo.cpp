#include "percentage.hpp"

double percentage::applyTo(double amount) const {
    return amount * value;
}