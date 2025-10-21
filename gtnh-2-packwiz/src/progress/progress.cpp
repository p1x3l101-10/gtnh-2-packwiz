#include "progress.hpp"

progress::progress(int capacity)
: realMax(capacity)
, impactOne(percentage::fromPercent(1).applyTo(capacity))
{}