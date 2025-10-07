#include <boost/asio/post.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include "config.hpp"
#include "gtnh2Packwiz/extras.hpp"
#include "gtnh2Packwiz/pack.hpp"
#include "gtnh2Packwiz/poolManager.hpp"
#include "loggerMacro.hpp"

extern boost::asio::thread_pool tp;

extern gtnh2Packwiz::poolManager pool;

namespace fs = std::filesystem;
using boost::asio::post;
using fs::path;
using std::string;

void gtnh2Packwiz::pack::build() {
    createLogger(logger);

    path packDir = CACHE "/pack";
    path configDir = CACHE "/config";

    // Create cache directory
    logger.debugStream() << "Created cache at: " << CACHE;
    fs::create_directories(CACHE);

    {
        path packZip = packDir.string() + ".zip";
        path configZip = configDir.string() + ".zip";

        // Download zips
        {
            string packUrl = config->getConfig().repo + "/archive/refs/heads/master.zip";
            string configUrl = config->getConfig().configRepo + "/archive/refs/tags/" + packVersion.string() + ".zip";
            logger.info("Downloading files...");
            auto tp = pool.getPool().lock();
            post(*tp, [packUrl, packZip]() { gtnh2Packwiz::extras::downloadFile(packUrl, packZip); });
            post(*tp, [configUrl, configZip]() { gtnh2Packwiz::extras::downloadFile(configUrl, configZip); });
            pool.reset();
            logger.info("Files downloaded.");
        }

        // Extract zips
        {
            logger.info("Extracting downloaded files...");
            auto tp = pool.getPool().lock();
            post(*tp, [packZip, packDir]() { gtnh2Packwiz::extras::extractZip(packZip, packDir); });
            post(*tp, [configZip, configDir]() { gtnh2Packwiz::extras::extractZip(configZip, configDir); });
            pool.reset();
            logger.info("Files extracted");
        }
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