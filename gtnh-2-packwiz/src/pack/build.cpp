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
using std::shared_ptr;

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
            // Exclude server files too
            if constexpr (EXCLUSIVLY_TARGET_CLIENT) {
                for (const auto &badFile: clientExclude) {
                    path realPath = destDir.string() + "/" + badFile;
                    logger.debugStream() << "Deleting file: '" << realPath.string() << "'";
                    fs::remove(realPath);
                }
            }
            // Remove the annoying .github directory
            fs::remove_all(destDir.string() + "/.github");
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
        // Add the unsup.toml file if support is enabled
        if constexpr (USING_UNSUP) {
            // Ensure that the file has been specified
            if (config->getUnsupConfig().configPath != "") {
                // Copy it in
                fs::copy_file(config->getUnsupConfig().configPath, destDir.string() + "/unsup.toml");
                // Track it
                files.push_back("unsup.toml");
            }
        }
        // Do the same for the unsup.ini file
        if constexpr (USING_UNSUP) {
            ofstream unsupINI(destDir.string() + "/unsup.ini");
            unsupINI
                << "version=1\n"
                << "source_format=packwiz\n"
                << "preset=minecraft\n"
                << "behavior=auto";
            if (config->getUnsupConfig().targetURL != "") {
                unsupINI << "source=" << config->getUnsupConfig().targetURL << "\n";
            } else {
                unsupINI
                    << "; Uncomment the following line and put the destination path\n"
                    << ";source=https://example.com/pack.toml\n";
            }
            if (config->getUnsupConfig().enableSigning) {
                unsupINI << "public_key=" << config->getUnsupConfig().publicKey;
            }
            if (config->getUnsupConfig().noGui) {
                unsupINI << "no_gui=true";
            }
            files.push_back("unsup.ini");
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
    {
        shared_ptr<json> gtnhAssets; // Oof, thats a big one
        json gtnhRelease;
        // These need to be downloaded and have a hash generated for them
        vector<toml::table> ghMods;
        vector<toml::table> cfMods;
        {
            // Load files
            ifstream gtnhAssetsFile(packDir.string() + "/gtnh-assets.json");
            gtnhAssetsFile >> *gtnhAssets;
            path releaseFile = packDir.string() + "/releases/manifests/" + packVersion.string() + ".json";
            ifstream gtnhReleaseFile(releaseFile);
            gtnhReleaseFile >> gtnhRelease;
        }
        // Generate meta files
        {
            path modDir = destDir.string() + "/mods";
            fs::create_directory(modDir);
            // Github mods
            for (const auto &ghMod : gtnhRelease["github_mods"].get<json::object_t>()) {
                json modVersion = gtnh2Packwiz::extras::getModVersion(gtnhAssets, ghMod.first, ghMod.second["version"]);
                toml::table modData;
                // Header data
                modData.insert_or_assign("name", ghMod.first);
                modData.insert_or_assign("filename", modVersion["filename"].get<json::string_t>());
                modData.insert_or_assign("side", gtnh2Packwiz::extras::convertSidedness(ghMod.second["side"]));

                // Download data
                toml::table modDownload;
                modDownload.insert_or_assign("url", modVersion["download_url"].get<json::string_t>());
                modDownload.insert_or_assign("hash-format", PACKWIZ_HASH_FORMAT);

                // Append tables
                modData.insert_or_assign("download", modDownload);

                // Add to list
                ghMods.push_back(modData);
            }
            // External mods
            for (const auto &cfMod : gtnhRelease["external_mods"].get<json::object_t>()) {
                json modVersion = gtnh2Packwiz::extras::getModVersion(gtnhAssets, cfMod.first, cfMod.second["version"]);
                toml::table modData;
                // Header data
                modData.insert_or_assign("name", cfMod.first);
                modData.insert_or_assign("filename", modVersion["filename"].get<json::string_t>());
                modData.insert_or_assign("side", gtnh2Packwiz::extras::convertSidedness(cfMod.second["side"]));

                // Download data
                toml::table modDownload;
                modDownload.insert_or_assign("mode", "metadata:curseforge");
                modDownload.insert_or_assign("hash-format", "sha1"); // CF API has its own hash format :(

                toml::table update;
                toml::table cfMetadata;
                cfMetadata.insert_or_assign("file-id", modVersion["curse_file"]["file_no"].get<json::string_t>());
                cfMetadata.insert_or_assign("project-id", modVersion["curse_file"]["project_no"].get<json::string_t>());
                update.insert_or_assign("curseforce", cfMetadata);

                // Append tables
                modData.insert_or_assign("download", modDownload);
                modData.insert_or_assign("update", update);

                // Add to list
                cfMods.push_back(modData);
            }
        }
        // Add meta files to index
    }
    // Write index
    {
        ofstream indexTOML(destDir.string() + "/index.toml");
        indexTOML << packwizIndex;
        logger.info("Wrote index.toml");
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
        logger.info("Wrote pack.toml");
    }
    // If unsup support is enabled and signatures are turned on, sign the files
    if constexpr (USING_UNSUP) {
        if (config->getUnsupConfig().enableSigning) {
            gtnh2Packwiz::extras::callSignify(destDir.string() + "/pack.toml", destDir.string() + "/unsup.sig" , config->getUnsupConfig().privateKeyPath);
            gtnh2Packwiz::extras::callSignify(destDir.string() + "/unsup.ini", destDir.string() + "/unsup.ini.sig" , config->getUnsupConfig().privateKeyPath);
            logger.info("Signed files for unsup");
        }
    }
    // Also, ditto on conditions, but create a JVMArgs file for bootstrap
    if constexpr (USING_UNSUP) {
        ofstream jvmArgs(destDir.string() + "bootstrapJvmArgs.txt");
        jvmArgs << "# The following line is the JVM args to allow unsup to download its own configuration\n";
        if (config->getUnsupConfig().targetURL != "") {
            jvmArgs << "-Dunsup.bootstrapUrl=" << config->getUnsupConfig().targetURL << "/unsup.ini";
        } else {
            jvmArgs << "# NOTE: a placeholder url is used, please set the config option `unsup.targetURL` when building\n";
            jvmArgs << "-Dunsup.bootstrapUrl=" << "http://example.com" << "/unsup.ini";
        }
        if (config->getUnsupConfig().enableSigning) {
            jvmArgs << " " << "-Dunsup.bootstrapKey='" << config->getUnsupConfig().publicKey << "'";
        }
        jvmArgs << "\n";
        logger.info("Wrote unsup bootstrap jvm args");
    }
    // Copy destDir to the destination
    {
        fs::copy(destDir, config->getConfig().outPath + "/dist", fs::copy_options::recursive);
        logger.info("Copied packwiz tree to output path");
        // Cleanup
        fs::remove_all(CACHE);
        logger.info("Cleared cache path");
        // Alert the user that we are done
        logger.notice("Build complete");
    }
}