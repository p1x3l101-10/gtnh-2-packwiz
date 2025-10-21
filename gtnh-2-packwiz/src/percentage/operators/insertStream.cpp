#include "percentage.hpp"
#include <ostream>
#include <iomanip>
#include <sstream>

using std::ostream;
using std::setprecision;
using std::stringstream;

ostream& operator<<(ostream& os, const percentage& p) {
    stringstream ss;
    ss << std::fixed << setprecision(2) << p.asPercent() << "%";
    os << ss.str();
    return os;
}