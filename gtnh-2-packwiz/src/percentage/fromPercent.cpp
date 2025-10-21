#include "percentage.hpp"

percentage percentage::fromPercent(double percentValue) {
    return percentage(percentValue / 100);
}