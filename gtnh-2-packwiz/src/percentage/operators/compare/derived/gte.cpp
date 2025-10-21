#include "percentage.hpp"

bool percentage::operator>=(const percentage& other) const {
    return !(*this < other);
}