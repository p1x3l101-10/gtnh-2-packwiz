#pragma once

#include <boost/asio/thread_pool.hpp>
#include <memory>

namespace gtnh2Packwiz {
    class poolManager {
        private:
            int threadCountNext;
            int threadCount;
            std::shared_ptr<boost::asio::thread_pool> pool = nullptr;
        public:
            poolManager(int maxThreads);
            ~poolManager();
            std::weak_ptr<boost::asio::thread_pool> getPool();
            void reset();
            const int getThreadCount();
            void setNewThreadCount(int count);
    };
} // namespace gtnh2Packwiz