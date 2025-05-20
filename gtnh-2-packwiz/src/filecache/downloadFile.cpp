#include "gtnh2Packwiz/filecache.hpp"
#include <exception>
#include <filesystem>
#include <fstream>
#include <future>
#include <boost/asio/post.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sys/fcntl.h>
#include "config.hpp"
#include "loggerMacro.hpp"

using std::string;

void gtnh2Packwiz::filecache::downloadFile() {
  createLogger(logger);
  if (file.valid()) {
    // Already downloading
    logger.debug("Already downloading");
    return;
  }
  std::promise<std::filesystem::path> promise;
  file = promise.get_future().share();
  // Multi-threaded to avoid waiting on downloads when we could be doing other things
  // Thread pool to avoid spamming the system with a kajilion threads (never again)
  boost::asio::post(gtnh2Packwiz::filecache::pool, [this, metadata = this->meta, digest = this->digest, promise = std::move(promise)]() mutable {
    createLogger(logger);
    try {
      std::ostringstream data;
      // Get file from the interwebz
      logger.debug("Downloading file...");
      curlpp::Easy request;
      request.setOpt<curlpp::options::Url>(metadata.uri);
      request.setOpt<curlpp::options::WriteStream>(&data);
      request.perform();
      // Write it to the cache
      logger.debug("Writing file to cache...");
      std::filesystem::path file = CACHE"/" + digest;
      std::ofstream fileStream;
      fileStream.open(file);
      fileStream << data.str();
      fileStream.close();
      // Write metadata
      meta.freshFile = true;
      meta.fetchTime = std::filesystem::last_write_time(file);
      // Fufill the promise
      logger.debug("Done");
      promise.set_value(file);
    } catch (...) {
      logger.error("Failure in downloading file");
      promise.set_exception(std::current_exception());
    }
  });
}