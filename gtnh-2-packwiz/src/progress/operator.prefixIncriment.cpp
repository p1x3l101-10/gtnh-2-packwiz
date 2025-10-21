#include "progress.hpp"

progress& progress::operator++() {
    tick();
    return *this;
}