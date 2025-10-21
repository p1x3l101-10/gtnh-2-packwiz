#pragma once

#include "percentage.hpp"

class progress {
    private:
        const int realMax;
        const int realTick = 1;
        const percentage impactOne;
        int realProgress = 0;
        percentage value = 0;
        percentage lastPrinted = 0;
    public:
        percentage minimumChange;
        progress(int capacity);
        progress& operator++();
        progress operator++(int);
        void tick();
        bool worthPrinting();
        percentage getPercent() const;
        friend std::ostream& operator<<(std::ostream& os, const progress& p);
};