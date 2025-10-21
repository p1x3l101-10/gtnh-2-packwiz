#include "percentage.hpp"
#include <ostream>
#include <iomanip>

using std::ostream;
using std::setprecision;
using std::fixed;

ostream& operator<<(ostream& os, const percentage& p) {
    os << fixed << setprecision(2) << p.asPercent() << "%";
    return os;
}