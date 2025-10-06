#include <log4cpp/Category.hh>
#include <memory>
#include <utility>
#include "config.hpp"
#include "gtnh2Packwiz/configFile.hpp"
#include "gtnh2Packwiz/init.hpp"
#include "gtnh2Packwiz/pack.hpp"
#include "gtnh2Packwiz/poolManager.hpp"

gtnh2Packwiz::poolManager pool(THREAD_POOL_MAX_THREADS);

int main(int c, char** v) {
  // Create the thread pool
  // Process args
  gtnh2Packwiz::init::argProcesser({c, v});
  // Set up logging
  gtnh2Packwiz::init::logger();
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
    pool.~poolManager(); // Terminate thread pool (manually call destructor)
    return 1;
  }
}