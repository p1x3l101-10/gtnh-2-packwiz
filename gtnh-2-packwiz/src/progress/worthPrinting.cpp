#include "progress.hpp"

bool progress::worthPrinting() {
    percentage mustSurpass = lastPrinted + minimumChange;
    if (mustSurpass > value) {
        lastPrinted = value;
        return true;
    }
    return false;
}