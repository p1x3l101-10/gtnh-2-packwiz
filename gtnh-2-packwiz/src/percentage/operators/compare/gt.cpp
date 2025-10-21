#include "percentage.hpp"

bool percentage::operator>(const percentage& other) const {
    return (value > other.value);
}