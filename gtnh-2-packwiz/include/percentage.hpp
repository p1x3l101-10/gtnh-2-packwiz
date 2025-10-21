#pragma once

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
        // Add a percentage
        void tick(float percent = 1);
        // Add a value specifically (useful for loops)
        void tick(long double amount);
        int getWhole();
        long double get();
        int getCapacity();
        // Precision is the digit to round to 0 is 1.0, 1 is 0.1, 2 is 0.01, etc.
        bool worthPrinting(int precision = 0);
};