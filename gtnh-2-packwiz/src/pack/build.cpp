#include <algorithm>
#include <boost/asio/post.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "config.hpp"
#include "gtnh2Packwiz/extras.hpp"
#include "gtnh2Packwiz/pack.hpp"
#include "gtnh2Packwiz/poolManager.hpp"
#include "loggerMacro.hpp"
#include <nlohmann/json.hpp>

extern gtnh2Packwiz::poolManager pool;

extern void shutdown(bool fatal = false, bool silent = false);

namespace fs = std::filesystem;
using boost::asio::post;
using fs::path;
using std::string;
using std::vector;
using nlohmann::json;
using std::ifstream;

template vector<string> gtnh2Packwiz::extras::setIntersection<string>(std::vector<string> a, std::vector<string> b);

void gtnh2Packwiz::pack::build() {
    createLogger(logger);

    path packDir = CACHE "/pack";
    path configDir = CACHE "/config";
    path destDir = CACHE "/packwiz";

    // Create cache directory
    {
        if (fs::exists(CACHE)) {
            logger.debugStream() << "Rereated cache at: " << CACHE;
            fs::remove_all(CACHE);
        } else {
            logger.debugStream() << "Created cache at: " << CACHE;
        }
        fs::create_directories(CACHE);
    }

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
    // Parse the modpack json for data
    json gtnhModpack;
    {
        path gtnhModpackJSONFile = packDir.string() + "/DreamAssemblerXXL-master/gtnh-modpack.json";
        ifstream gtnhModpackJSON(gtnhModpackJSONFile);
        gtnhModpackJSON >> gtnhModpack;
    }

    // Create the basic file structure
    {
        // Config has the raw files already
        {
            logger.debug("Coping config repo to destination");
            // Copy files
            path realCFG = configDir.string() + "/GT-New-Horizons-Modpack-" + packVersion.string();
            fs::copy(realCFG, destDir);
        }
        // Find files that are excluded in both client and server so we can delete them
        {
            logger.info("Deleting files excluded on server and client");
            // Sanity checks
            if (! gtnhModpack["server_exclusions"].is_array()) {
                logger.fatal("Expected 'server_exclusions' to be an array!");
                shutdown(true);
            }
            if (! gtnhModpack["client_exclusions"].is_array()) {
                logger.fatal("Expected 'client_exclusions' to be an array!");
                shutdown(true);
            }
            // Convert into useful types
            vector<string> serverExclude = gtnhModpack["server_exclusions"].get<vector<string>>();
            vector<string> clientExclude = gtnhModpack["client_exclusions"].get<vector<string>>();
            // Get the intersection
            vector<string> allExclude = gtnh2Packwiz::extras::setIntersection<string>(serverExclude, clientExclude);
            // Delete files that match the intersection
            for (const auto &badFile : allExclude) {
                path realPath = destDir.string() + "/" + badFile;
                logger.debugStream() << "Deleting file: '" << realPath.string() << "'";
                fs::remove(realPath);
            }
        }
    }
    // Construct an index toml
    
    // OLD TODOS
    // Parse version
    // Get files from config repo
    // Read mods to metadata container
    // Serialize container to packwiz metafiles
    // Do the same for everything in the config repo
    // Write an unsup config file
    // Hash everything and create an index.toml file
    // Create the pack.toml file
}