#include <log4cpp/Category.hh>
#include <memory>
#include <utility>
#include "config.hpp"
#include "gtnh2Packwiz/configFile.hpp"
#include "gtnh2Packwiz/init.hpp"
#include "gtnh2Packwiz/pack.hpp"
#include "gtnh2Packwiz/poolManager.hpp"
#include <indicators/cursor_control.hpp>

gtnh2Packwiz::poolManager pool(THREAD_POOL_MAX_THREADS);

void shutdown(bool fatal = false, bool silent = false) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".main.shutdown");
    if (!silent) {
        logger.alert("Shutting down...");
    }
    if (fatal) {
        exit(1);
    }
    exit(0);
}

extern "C" void sigHandler(int sig) {
    shutdown();
}

int main(int c, char** v) {
    // Register signal handlers
    std::signal(SIGINT, sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGHUP, sigHandler);
    // Setup
    gtnh2Packwiz::init::argProcesser({c, v});
    gtnh2Packwiz::init::ftxui();
    gtnh2Packwiz::init::logger();
    // Init
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".main");
    logger.debug("Hierarchical application logging set up.");
    std::shared_ptr<gtnh2Packwiz::configFile> config = nullptr;
    if (gtnh2Packwiz::init::args.count("config")) {
        logger.debug("Loading from user-specified config file");
        config = std::make_shared<gtnh2Packwiz::configFile>(gtnh2Packwiz::init::args["config"].as<std::string>());
    } else {
        logger.debug("Loading default config file");
        config = std::make_shared<gtnh2Packwiz::configFile>(DEFAULT_ADMIN_CONFIG_PATH);
    }
    if (gtnh2Packwiz::init::args.count("pack-version")) {
        gtnh2Packwiz::pack pack(gtnh2Packwiz::init::args["pack-version"].as<std::string>(), config);
        pack.build();
    } else {
        logger.fatal("You need to specify a version of the pack to build!");
        shutdown(true);
    }
    shutdown();
}