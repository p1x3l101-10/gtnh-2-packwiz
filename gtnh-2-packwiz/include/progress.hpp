#pragma once

#include "percentage.hpp"
#include <ostream>

class progress {
    private:
        const int realMax;
        const int realTick = 1;
        const percentage impactOne;
        percentage value;
        percentage oldValue;
};