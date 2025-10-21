#pragma once

#include <iomanip>
#include <ostream>

class percentage {
    private:
        const int capacity;
        const long double onePercent;
        long double value = 0;
        long double compairedValue = 0;
        long double prevValue = 0;
        long double roundToPrecision(long double value, int decimalPlaces);
    public:
        percentage(int capacity);
        int defaultPrecision = 0;
        // Add a percentage
        void setPercent(float percent = 1);
        // Add a value specifically (useful for loops)
        void addValue(long double amount);
        void tick();
        void setProgress(int progress);
        int getWhole() const;
        long double get(int precision = 0) const;
        int getCapacity() const;
        // Precision is the digit to round to 0 is 1.0, 1 is 0.1, 2 is 0.01, etc.
        bool worthPrinting(int precision = 0) const;
        // Easy printing
        friend std::ostream& operator<<(std::ostream& os, const percentage& p) {
            os << std::fixed << std::setprecision(p.defaultPrecision) << p.get(p.defaultPrecision) << "%";
            return os;
        };
};