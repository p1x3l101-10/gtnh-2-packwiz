#include "progress.hpp"

progress::progress(int capacity)
: realMax(capacity)
, impactOne(1 / static_cast<double>(capacity))
, minimumChange(0.001)
{}