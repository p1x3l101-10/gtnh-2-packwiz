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
#include <boost/program_options/variables_map.hpp>

// Toml is being dump
using std::optional;
#include <toml++/impl/table.hpp>
#include <toml++/impl/array.hpp>

extern gtnh2Packwiz::poolManager pool;
extern boost::program_options::variables_map args;
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
        if (! fs::exists(CACHE)) {
            logger.debugStream() << "Created cache at: " << CACHE;
            fs::create_directories(CACHE);
        }
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
    // Redo the path variables for the new paths
    packDir = packDir.string() + "/DreamAssemblerXXL-master";
    configDir = configDir.string() + "/GT-New-Horizons-Modpack-" + packVersion.string();
    // Parse the modpack json for data
    json gtnhModpack;
    {
        path gtnhModpackJSONFile = packDir.string() + "/gtnh-modpack.json";
        ifstream gtnhModpackJSON(gtnhModpackJSONFile);
        gtnhModpackJSON >> gtnhModpack;
    }

    // Create the basic file structure
    {
        // Config has the raw files already
        {
            // ALWAYS regenerate my dist dir
            if (fs::exists(destDir)) {
                logger.debug("Removing old dest dir");
                fs::remove_all(destDir);
            }
            logger.debug("Coping config repo to destination");
            // Copy files
            fs::copy(configDir, destDir, fs::copy_options::recursive);
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
                << "behavior=auto\n";
            if (config->getUnsupConfig().targetURL != "") {
                unsupINI << "source=" << config->getUnsupConfig().targetURL << "/pack.toml" << "\n";
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
            int onePercent = files.size() / 100;
            int current = 0;
            int progress = 0;
            for (const auto &file : files) {
                current++;
                if (current >= onePercent) {
                    current = 0;
                    progress++;
                    logger.infoStream() << "Hashed " << progress << "\% of files";
                }
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
        vector<toml::table> mods;
        // One percent of the mods
        int onepercentMods;
        {
            // Load files
            logger.info("Loading pack metadata from JSON");
            logger.debug("Loading the very large file that is the gtnh-assets.json");
            ifstream gtnhAssetsFile(packDir.string() + "/gtnh-assets.json");
            gtnhAssets = std::make_shared<json>(json::parse(gtnhAssetsFile));
            logger.debug("Loading the release manifest");
            path releaseFile = packDir.string() + "/releases/manifests/" + packVersion.string() + ".json";
            ifstream gtnhReleaseFile(releaseFile);
            gtnhReleaseFile >> gtnhRelease;
            // Calculate percentage for progress
            onepercentMods = (gtnhRelease["github_mods"].get<json::object_t>().size() + gtnhRelease["external_mods"].get<json::object_t>().size()) / 100;
        }
        // Generate meta files
        {
            path modDir = destDir.string() + "/mods";
            fs::create_directory(modDir);
            logger.debug("Generating packwiz metafiles for github mods");
            int progress = 0;
            int current = 0;
            // Github mods
            for (const auto &ghMod : gtnhRelease["github_mods"].get<json::object_t>()) {
                logger.debugStream() << "Generating metadata for mod: '" << ghMod.first << "'";
                // Calc the percentage things
                current++;
                if  (current >= onepercentMods) {
                    current = 0;
                    progress++;
                    logger.infoStream() << "Percentage calculated: " << progress << "%";
                }
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

                // Extra data
                toml::table extraData;
                extraData.insert_or_assign("version", ghMod.second["version"].get<json::string_t>());

                // Append tables
                modData.insert_or_assign("download", modDownload);
                modData.insert_or_assign("x-generator", extraData);

                // Add to list
                mods.push_back(modData);
            }
            logger.debug("Generating packwiz metafiles for curseforge mods");
            // External mods
            for (const auto &exMod : gtnhRelease["external_mods"].get<json::object_t>()) {
                // Calc the percentage things
                current++;
                if  (current >= onepercentMods) {
                    current = 0;
                    progress++;
                    logger.infoStream() << "Percentage calculated: " << progress << "%";
                }
                logger.debugStream() << "Generating metadata for mod: '" << exMod.first << "'";
                json modVersion = gtnh2Packwiz::extras::getModVersion(gtnhAssets, exMod.first, exMod.second["version"]);
                toml::table modData;
                // Header data
                modData.insert_or_assign("name", exMod.first);
                modData.insert_or_assign("filename", modVersion["filename"].get<json::string_t>());
                modData.insert_or_assign("side", gtnh2Packwiz::extras::convertSidedness(exMod.second["side"]));

                // Download data
                toml::table modDownload;
                modDownload.insert_or_assign("url", modVersion["download_url"].get<json::string_t>());
                modDownload.insert_or_assign("hash-format", PACKWIZ_HASH_FORMAT);

                // Append tables
                modData.insert_or_assign("download", modDownload);

                // Add to list
                mods.push_back(modData);
            }
        }
        logger.info("Metadata generated");
        // Generate hashes
        {
            int current = 0;
            int progress = 0;
            vector<packwizFileEntry> indexFiles; // New index file
            logger.info("Generating hashes for mod files");
            {
                path tempPath = CACHE "/modDownloads";
                path apiCache = CACHE "/apiCache";
                if (!fs::exists(tempPath)) {
                    fs::create_directory(tempPath);
                }
                if (!fs::exists(apiCache)) {
                    fs::create_directory(apiCache);
                }
                for (int i = 0; i < mods.size(); i++) {
                    // Calc the percentage things
                    current++;
                    if  (current >= onepercentMods) {
                        current = 0;
                        progress++;
                        logger.infoStream() << "Percentage calculated: " << progress << "%";
                    }
                    const auto &mod = mods.at(i);
                    logger.debugStream() << "Current mod: '" << mod.at_path("name").ref<string>() << "'";
                    string dlURL = mod.at_path("download.url").ref<string>();
                    // Check if the url is a redirect json
                    if (dlURL.contains("api.github.com")) {
                        // If it contains a redirect json, download it and get the real URL
                        path dlPath = apiCache.string() + "/" + mod.at_path("filename").ref<string>();
                        logger.debug("This mod's download is behind an api");
                        extras::downloadFile(dlURL, dlPath, true, { false, true });
                        json apiResponce;
                        {
                            ifstream responceFile(dlPath);
                            responceFile >> apiResponce;
                        }
                        // Send an error if GH responds with 404 instead of just failing
                        if (apiResponce.contains("status")) {
                            string status = apiResponce["status"].get<json::string_t>();
                            if (status == "404") {
                                logger.warnStream() << "Mod: '" << mod.at_path("name").ref<string>() << "' does not exist on github!";
                                json modVersion = gtnh2Packwiz::extras::getModVersion(gtnhAssets, mod.at_path("name").ref<string>(), mod.at_path("x-generator.version").ref<string>());
                                string dlURL = modVersion["download_url"].get<json::string_t>();
                                mods.at(i)["download"].as_table()->insert_or_assign("url", dlURL);
                                goto forceRegen;
                                continue;
                            }
                        }
                        string realUrl = apiResponce["browser_download_url"].get<json::string_t>();
                        // Update the table
                        mods.at(i)["download"].as_table()->insert_or_assign("url", realUrl);
                        // Dont do extra work if github provides the hash
                        if (!apiResponce["digest"].is_null()) {
                            logger.debug("Hash is provided, skipping recalculation...");
                            string digest = apiResponce["digest"].get<json::string_t>();
                            auto delim = digest.find(':');
                            string digestType = digest.substr(0, delim);
                            digest.erase(0, delim + 1);
                            // Update table
                            mods.at(i)["download"].as_table()->insert_or_assign("hash-format", digestType);
                            mods.at(i)["download"].as_table()->insert_or_assign("hash", digest);
                        } else {
                            // Need the hash anyways
                            path dlPath = tempPath.string() + "/" + mod.at_path("filename").ref<string>();
                            string hash = gtnh2Packwiz::extras::generatePWHash(dlPath, PACKWIZ_HASH_FORMAT);
                            // Add the hash to the packwiz file
                            mods.at(i)["download"].as_table()->insert_or_assign("hash", hash);
                        }
                    } else {
                        forceRegen:
                        // Needs the hash to be manually generated
                        path dlPath = tempPath.string() + "/" + mod.at_path("filename").ref<string>();
                        extras::downloadFile(dlURL, dlPath, true, { false, true });
                        logger.debug("Generating hash");
                        string hash = gtnh2Packwiz::extras::generatePWHash(dlPath, PACKWIZ_HASH_FORMAT);
                        // Add the hash to the packwiz file
                        mods.at(i)["download"].as_table()->insert_or_assign("hash", hash);
                    }
                }
            }
        }
        // Add meta files to index
        if (! fs::exists(destDir.string() + "/mods")) {
            fs::create_directory(destDir.string() + "/mods");
        }
        {
            toml::array fileArray = packwizIndex.at_path("files").ref<toml::array>();
            for (const auto &entry : mods) {
                toml::table file;
                {
                    ofstream metaToml(destDir.string() + "/mods/" + entry.at_path("name").ref<string>() + ".pw.toml");
                    metaToml << entry;
                }
                string localPath = "mods/" + entry.at_path("name").ref<string>() + ".pw.toml";
                file.insert_or_assign("file", localPath);
                file.insert_or_assign("hash", entry.at_path("download.hash").ref<string>());
                file.insert_or_assign("metafile", true);
                fileArray.push_back(file);
                logger.debugStream() << "Wrote metadata file for mod: '" << entry.at_path("name").ref<string>() << "'";
            }
            logger.info("Wrote metadata files to the index");
            // Add to the index
            packwizIndex.insert_or_assign("files", fileArray);
        }
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
        ofstream jvmArgs(destDir.string() + "/bootstrapJvmArgs.txt");
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
        path output;
        // Avoid clobbering the workdir
        if (config->getConfig().outPath == ".") {
            output = config->getConfig().outPath + "/dist";
        } else {
            output = config->getConfig().outPath;
        }
        // Fix a bug when the destdir already exists
        if (fs::exists(output)) {
            logger.warn("Destination already exists, proceding anyways and removing it");
            fs::remove_all(output);
        }
        fs::copy(destDir, output, fs::copy_options::recursive);
        logger.info("Copied packwiz tree to output path");
        // Cleanup
        logger.debug("Cleaned workdir");
        fs::remove_all(destDir);
        if (args.count("clear-cache")) {
            fs::remove_all(CACHE);
            logger.info("Cleared cache path");
        }
        // Alert the user that we are done
        logger.notice("Build complete");
    }
}