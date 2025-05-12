#include <utility>
#include <log4cpp/Category.hh>
#include "gtnh2Packwiz/init.hpp"
#include "config.hpp"

int main(int c, char** v) {
  // Process args
  gtnh2Packwiz::init::argProcesser({c, v});
  // Set up logging
  gtnh2Packwiz::init::logger();
  log4cpp::Category& logger = log4cpp::Category::getInstance(NAME".main");
  logger.debug("Hierarchical application logging set up.");
}