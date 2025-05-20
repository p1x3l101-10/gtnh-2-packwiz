#define INIT_VARS
#include <memory>
#include <utility>
#include <log4cpp/Category.hh>
#include "gtnh2Packwiz/configFile.hpp"
#include "gtnh2Packwiz/init.hpp"
#include "config.hpp"

int main(int c, char** v) {
  // Process args
  gtnh2Packwiz::init::argProcesser({c, v});
  // Set up logging
  gtnh2Packwiz::init::logger();
  log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".main");
  logger.debug("Hierarchical application logging set up.");
  std::shared_ptr<gtnh2Packwiz::configFile> config = nullptr;
  if (gtnh2Packwiz::init::args.count("config")) {
    logger.debug("Loading from user-specified config file");
    config = std::make_shared<gtnh2Packwiz::configFile>(gtnh2Packwiz::init::args["config"].as<std::string>());
  } else {
    logger.debug("Loading default config file");
    config = std::make_shared<gtnh2Packwiz::configFile>(DEFAULT_ADMIN_CONFIG_PATH);
  }
}