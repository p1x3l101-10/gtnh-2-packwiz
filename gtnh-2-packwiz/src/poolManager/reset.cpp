#include "gtnh2Packwiz/poolManager.hpp"

void gtnh2Packwiz::poolManager::reset() {
    pool->wait();
    pool->join();
    pool.reset();
    threadCount = threadCountNext;
    pool = std::make_shared<boost::asio::thread_pool>(threadCount);
}