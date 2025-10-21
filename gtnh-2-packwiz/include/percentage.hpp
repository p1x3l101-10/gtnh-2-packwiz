#pragma once

#include <ostream>

class percentage {
    private:
        double value;
    public:
        percentage(double decimalValue = 0.0);
        static percentage fromPercent(double percentValue);
        double asDecimal() const;
        double asPercent() const;
        double applyTo(double amount) const;
        // Operator overloads
        percentage operator+(const percentage& other) const;
        percentage operator-(const percentage& other) const;
        double operator*(const double& other) const;
        percentage& operator=(const int& value);
        percentage& operator=(const double& value);
        percentage& operator=(const percentage& other);
        operator double() const;
        bool operator>(const percentage& other) const;
        bool operator<=(const percentage& other) const;
        bool operator<(const percentage& other) const;
        bool operator>=(const percentage& other) const;
        bool operator==(const percentage& other) const;
        bool operator!=(const percentage& other) const;
        friend std::ostream& operator<<(std::ostream& os, const percentage& p);
};