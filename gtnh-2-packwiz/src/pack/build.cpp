#include <filesystem>
#include <string>
#include "gtnh2Packwiz/pack.hpp"
#include "config.hpp"
#include "gtnh2Packwiz/extras.hpp"
#include "loggerMacro.hpp"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

extern boost::asio::thread_pool tp;

namespace fs = std::filesystem;
using std::string;
using fs::path;
using boost::asio::post;

void gtnh2Packwiz::pack::build() {
  createLogger(logger);
  // Create cache directory
  logger.debugStream() << "Created cache at: " << CACHE;
  fs::create_directories(CACHE);
  // Download zips
  path packZip = CACHE"/pack.zip";
  path configZip = CACHE"/config.zip";
  {
    string packUrl = config->getConfig().repo + "/archive/refs/heads/master.zip";
    string configUrl = config->getConfig().configRepo + "/archive/refs/tags/" + packVersion.string() + ".zip";
    logger.info("Downloading files");
    post(tp, [packUrl, packZip](){gtnh2Packwiz::extras::downloadFile(packUrl, packZip);});
    post(tp, [configUrl, configZip](){gtnh2Packwiz::extras::downloadFile(configUrl, configZip);});
    tp.join();
    logger.info("Files downloaded.");
  }
  // Parse version
  // Get files from config repo
  // Read mods to metadata container
  // Serialize container to packwiz metafiles
  // Do the same for everything in the config repo
  // Write an unsup config file
  // Hash everything and create an index.toml file
  // Create the pack.toml file
}