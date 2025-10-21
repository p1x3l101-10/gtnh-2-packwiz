#include "progress.hpp"

bool progress::worthPrinting(int precision) {
    long double current = roundToPrecision(value, precision);
    long double old = roundToPrecision(prevValue, precision);
    // If the current value is bigger
    if (current > old) {
        // Ensure we cant print twice in rapid succession
        long double ensureNoDoublePrinting = roundToPrecision(compairedValue, precision);
        if (current != ensureNoDoublePrinting) {
            compairedValue = value;
            return true;
        }
    }
    return false;
}