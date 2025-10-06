#include "gtnh2Packwiz/poolManager.hpp"

gtnh2Packwiz::poolManager::~poolManager() {
    pool->stop();
    pool->join();
}