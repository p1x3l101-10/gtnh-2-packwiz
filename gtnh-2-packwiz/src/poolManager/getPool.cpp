#include <memory>
#include "gtnh2Packwiz/poolManager.hpp"

std::weak_ptr<boost::asio::thread_pool> gtnh2Packwiz::poolManager::getPool() {
    return pool;
}