#include "percentage.hpp"

percentage percentage::operator=(const percentage& other) {
    if (this != &other) {
        value = other.value;
    }
}