#include "gtnh2Packwiz/poolManager.hpp"

gtnh2Packwiz::poolManager::poolManager(int maxThreads)
: threadCount(maxThreads)
, threadCountNext(maxThreads)
, pool(std::make_shared<boost::asio::thread_pool>(maxThreads))
{};