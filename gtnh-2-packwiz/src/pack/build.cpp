#include <boost/asio/post.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include "config.hpp"
#include "gtnh2Packwiz/extras.hpp"
#include "gtnh2Packwiz/pack.hpp"
#include "gtnh2Packwiz/poolManager.hpp"
#include "loggerMacro.hpp"
#include <indicators/multi_progress.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/dynamic_progress.hpp>

extern boost::asio::thread_pool tp;

extern gtnh2Packwiz::poolManager pool;

namespace fs = std::filesystem;
using boost::asio::post;
using fs::path;
using std::string;
namespace pb = indicators;
namespace pbo = pb::option;
using pb::FontStyle;
using pb::Color;
using pb::ProgressBar;
using pb::DynamicProgress;

constexpr std::unique_ptr<ProgressBar> makeBar(string text) {
    return std::make_unique<ProgressBar>(
        pbo::ForegroundColor{Color::white},
        pbo::ShowElapsedTime{true},
        pbo::ShowRemainingTime{true},
        pbo::PrefixText{text},
        pbo::FontStyles {
            std::vector<FontStyle>{
                FontStyle::bold
            }
        }
    );
}

void gtnh2Packwiz::pack::build() {
    createLogger(logger);
    DynamicProgress<ProgressBar> bars;
    bars.set_option(pbo::HideBarWhenComplete{false});

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
            auto bar1 = makeBar("Pack archive");
            bars.push_back(std::move(bar1));
            post(*tp, [packUrl, packZip, &bars]() { gtnh2Packwiz::extras::downloadFile(packUrl, packZip, 0, &bars); });
            auto bar2 = makeBar("Config archive");
            bars.push_back(std::move(bar2));
            post(*tp, [configUrl, configZip, &bars]() { gtnh2Packwiz::extras::downloadFile(configUrl, configZip, 0, &bars); });
            pool.reset();
            logger.info("Files downloaded.");
        }

        // Extract zips
        {
            logger.info("Extracting downloaded files...");
            auto tp = pool.getPool().lock();
            auto bar1 = makeBar("Extracting pack");
            bars.push_back(std::move(bar1));
            post(*tp, [packZip, packDir, &bars]() { gtnh2Packwiz::extras::extractZip(packZip, packDir, 0, &bars); });
            auto bar2 = makeBar("Extracting config");
            bars.push_back(std::move(bar2));
            post(*tp, [configZip, configDir, &bars]() { gtnh2Packwiz::extras::extractZip(configZip, configDir, 1, &bars); });
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