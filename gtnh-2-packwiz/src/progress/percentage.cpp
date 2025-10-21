#include "progress.hpp"

progress::progress(int capacity)
: capacity(capacity)
, onePercent(static_cast<long double>(capacity) / 100)
, value(0)
{}