#include "percentage.hpp"

percentage percentage::operator=(const double& other) {
    if (this->value != other) {
        value = other;
    }
    return *this;
}