#include "progress.hpp"

void progress::tick() {
    realProgress++;
    value = value + impactOne;
}