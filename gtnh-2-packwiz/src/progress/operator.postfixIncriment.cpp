#include "progress.hpp"

progress progress::operator++(int) {
    progress old = *this;
    tick();
    return old;
}