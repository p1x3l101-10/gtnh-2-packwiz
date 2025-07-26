#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include "gtnh2Packwiz/init.hpp"
#include "gtnh2Packwiz/loggerLayout.hpp"

void gtnh2Packwiz::init::logger() {
  log4cpp::Appender *appender;
  if (gtnh2Packwiz::init::args.count("logfile")) {
    appender = new log4cpp::FileAppender("default", args["logfile"].as<std::string>());
  } else {
    appender = new log4cpp::OstreamAppender("console", &std::cout);
  }
  appender->setLayout(new gtnh2Packwiz::loggerLayout());
  log4cpp::Category &root = log4cpp::Category::getRoot();
  root.setAppender(appender);
  try { // Set loglevel
    root.setPriority(gtnh2Packwiz::init::getPriority());
  } catch (std::runtime_error &e) {
    root.error(e.what());
    throw;
  }
  root.debug("Logging initialized");
}