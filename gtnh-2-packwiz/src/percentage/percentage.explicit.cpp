#include "percentage.hpp"

percentage::percentage(double decimalValue)
: value(decimalValue)
{
    // Ensure value clamps
    if (value < 0.0) value = 0.0;
    if (value > 1.0) value = 1.0;
}