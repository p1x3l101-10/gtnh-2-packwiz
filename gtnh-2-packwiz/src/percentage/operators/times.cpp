#include "percentage.hpp"

double percentage::operator*(const double& other) const {
    return applyTo(other);
}