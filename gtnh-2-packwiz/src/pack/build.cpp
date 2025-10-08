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

// Toml is being dump
using std::optional;
#include <toml++/impl/table.hpp>
#include <toml++/impl/array.hpp>

extern gtnh2Packwiz::poolManager pool;

[[noreturn]] void shutdown(bool fatal = false, bool silent = false);

namespace fs = std::filesystem;
using boost::asio::post;
using fs::path;
using std::string;
using std::vector;
using nlohmann::json;
using std::ifstream;
using std::ofstream;

template vector<string> gtnh2Packwiz::extras::setIntersection<string>(std::vector<string> a, std::vector<string> b);

struct packwizFileEntry {
    string file;
    string hash;
    bool metafile = false;
};

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

        // Cleanup
        {
            logger.debug("Cleaning used zip files");
            fs::remove(packZip);
            fs::remove(configZip);
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
            fs::copy(realCFG, destDir, fs::copy_options::recursive);
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
        // Cleanup
        {
            logger.debug("Removing config directory as it isnt used anymore");
            fs::remove_all(configDir);
        }
    }
    // Construct the index toml
    toml::table packwizIndex;
    {
        vector<path> files; // Temp var to track all found files
        {
            // Logger for my lambda
            log4cpp::Category& fileFinderLogger = log4cpp::Category::getInstance(logger.getName() + ".fileFinder");
            // Create a recursive function (recursion is a bit iffy, but whatever)
            // Explicitly mark as a void return to avoid any wierdness with auto
            // TODO: Find a better way to do this
            auto fileFinder = [&files, &logger = fileFinderLogger](this auto&& self, path baseDir) -> void {
                for (const auto &entry : fs::directory_iterator(baseDir)) {
                    if (entry.is_regular_file()) {
                        logger.debugStream() << "Found file: '" << static_cast<path>(entry).string() << "'";
                        files.push_back(entry);
                    } else if (entry.is_directory()) {
                        logger.debugStream() << "Recursing into directory: '" << static_cast<path>(entry).string() << "'";
                        self(entry);
                    } else {
                        logger.fatal("Unknown file found!");
                        logger.fatalStream() << "File is: '" << static_cast<path>(entry).string() << "'";
                        shutdown(true);
                    }
                }
            };
            // Call my recursive function
            fileFinder(destDir);
        }
        // Create the hashes needed
        vector<packwizFileEntry> indexFiles;
        {
            // Assign hash format
            packwizIndex.insert_or_assign("hash-format", PACKWIZ_HASH_FORMAT);
            int baseLength = destDir.string().length() + 1; // Add one to account for trailing '/'
            // NOTE: None of these files will be metadata files
            for (const auto &file : files) {
                string hash = gtnh2Packwiz::extras::generatePWHash(file, PACKWIZ_HASH_FORMAT);
                string fileBasename = file.string().erase(0, baseLength);
                indexFiles.push_back({
                    fileBasename,
                    hash,
                    false
                });
            }
        }
        // Convert custom struct into a toml table for the index
        {
            logger.info("Merging file vector into index file array");
            toml::array fileArray;
            for (const auto &entry : indexFiles) {
                toml::table file;
                file.insert_or_assign("file", entry.file);
                file.insert_or_assign("hash", entry.hash);
                if (entry.metafile == true) {
                    file.insert_or_assign("metafile", entry.metafile);
                }
                fileArray.push_back(file);
            }
            // Add to the index
            packwizIndex.insert_or_assign("files", fileArray);
        }
    }

    // Find downloads and write the meta files
    // Add meta files to index
    // Write index
    {
        ofstream indexTOML(destDir.string() + "/index.toml");
        indexTOML << packwizIndex;
    }
    // Write pack.toml
    {
        toml::table pack;
        // Write data
        // Surely there must be a better way to do this????
        pack.insert_or_assign("name", "GregTech: New Horizons"); // if GTNH ever changes the name, then change this too
        pack.insert_or_assign("author", GTNH_AUTHOR);
        pack.insert_or_assign("version", packVersion.string());
        pack.insert_or_assign("pack-format", "packwiz:1.1.0"); // This is the format version I am targeting
        {
            toml::table index;
            index.insert_or_assign("file", "index.toml");
            index.insert_or_assign("hash-format", PACKWIZ_HASH_FORMAT);
            index.insert_or_assign("hash", gtnh2Packwiz::extras::generatePWHash(destDir.string() + "/index.toml", PACKWIZ_HASH_FORMAT));
            pack.insert_or_assign("index", index);
        }
        {
            toml::table versions;
            versions.insert_or_assign("forge", "");
            versions.insert_or_assign("minecraft", "1.7.10");
            if constexpr (USING_UNSUP) {
                versions.insert_or_assign("unsup", UNSUP_VERSION);
            }
        }
        {
            ofstream packTOML(destDir.string() + "/pack.toml");
            packTOML << pack;
        }
    }
    // If unsup support is enabled, generate the files for that too
    // Copy destDir to the destination
}