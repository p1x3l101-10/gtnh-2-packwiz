#pragma once
#include <filesystem>
#include <future>
#include <string>
#include <nlohmann/json.hpp>
#include <log4cpp/Category.hh>
#include <boost/asio/thread_pool.hpp>
#include "config.hpp"

namespace gtnh2Packwiz {
  class filecache {
    protected:
      struct metadataStruct {
        std::string uri;
        std::filesystem::file_time_type fetchTime;
        bool freshFile;
      };
    private:
      static boost::asio::thread_pool pool;
      bool fileReady = false;
      std::string digest;
      metadataStruct meta;
      log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".filecache");
      void downloadFile();
      std::shared_future<std::filesystem::path> file;
    public:
      filecache(std::string uri);
      void forceRefresh();
      nlohmann::json get() const;
      metadataStruct getMeta() const;
      std::filesystem::path getFile() const;
      std::shared_future<std::filesystem::path> getFuture() const;
  };
}

#ifdef INIT_VARS
boost::asio::thread_pool gtnh2Packwiz::filecache::pool{4};
#endif