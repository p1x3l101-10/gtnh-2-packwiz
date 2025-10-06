#include <filesystem>
#include <string>
#include "gtnh2Packwiz/pack.hpp"
#include "config.hpp"
#include "gtnh2Packwiz/extras.hpp"
#include "loggerMacro.hpp"

namespace fs = std::filesystem;
using std::string;
using fs::path;

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
    gtnh2Packwiz::extras::downloadFile(packUrl, packZip);
    gtnh2Packwiz::extras::downloadFile(configUrl, configZip);
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