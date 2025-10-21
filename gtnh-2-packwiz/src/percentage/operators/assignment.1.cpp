#include "percentage.hpp"

percentage& percentage::operator=(const int& other) {
    if (this->value != other) {
        // Clamps
        if (other >= 1) {
            value = 1;
        } else {
            value = 0;
        }
    }
    return *this;
}