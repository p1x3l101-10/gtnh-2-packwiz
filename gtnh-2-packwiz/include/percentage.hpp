#pragma once

#include <iomanip>
#include <ostream>

class percentage {
    private:
        double value;
    public:
        explicit percentage(double decimalValue = 0.0);
        static percentage fromPercent(double percentValue);
        double asDecimal() const;
        double asPercent() const;
        percentage operator+(const percentage& other) const;
        percentage operator-(const percentage& other) const;
        double applyTo(double amount) const;
        friend std::ostream& operator<<(std::ostream& os, const percentage& p) {
            os << std::fixed << std::setprecision(2) << p.asPercent() << "%";
            return os;
        }
};