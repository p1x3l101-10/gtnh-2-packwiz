#include "percentage.hpp"

percentage percentage::operator-(const percentage& other) const {
    return percentage(value - other.value);
}