#include "progress.hpp"
#include "config.hpp"

bool progress::worthPrinting() {
    if constexpr (!PRINT_PERCENTAGES) {
        // Completely disable print logic
        return false;
    }
    percentage mustSurpass = lastPrinted + minimumChange;
    if (mustSurpass < value) {
        lastPrinted = value;
        if constexpr (INTERCEPT_PRINTING_PROGRESS_FOR_BAR) {} else {
            return true;
        }
    }
    return false;
}