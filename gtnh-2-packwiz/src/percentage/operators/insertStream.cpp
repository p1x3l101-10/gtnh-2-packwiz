#include "percentage.hpp"
#include <ostream>
#include <iomanip>

using std::ostream;
using std::setprecision;

ostream& operator<<(ostream& os, const percentage& p) {
    std::streamsize originalPrecision = os.precision();
    os << setprecision(2) << p.asPercent() << "%" << setprecision(originalPrecision);
    return os;
}