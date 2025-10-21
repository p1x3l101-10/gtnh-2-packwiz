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
        percentage minimumChange = percentage::fromPercent(0.1);
        progress(int capacity);
        progress& operator++();
        void tick();
        bool worthPrinting();
        percentage getPercent() const;
};