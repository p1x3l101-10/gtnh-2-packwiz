#include "percentage.hpp"

percentage& percentage::operator=(const percentage& other) {
    this->value = other.value;
    return *this;
}