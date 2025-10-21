#include "progress.hpp"

progress& progress::operator++() {
    tick();
}