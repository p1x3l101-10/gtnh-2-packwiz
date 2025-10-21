#include "progress.hpp"

std::ostream& operator<<(std::ostream& os, const progress& p) {
    os << p.value;
    return os;
}