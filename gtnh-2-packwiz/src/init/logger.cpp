#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include "gtnh2Packwiz/init.hpp"
#include "gtnh2Packwiz/loggerLayout.hpp"
#include <rang/rang.hpp>

using rang::setControlMode;
using rang::control;
using std::cout;
using log4cpp::OstreamAppender;
using log4cpp::FileAppender;
using log4cpp::Category;
using log4cpp::Appender;

void gtnh2Packwiz::init::logger() {
    // Setup colors
    setControlMode(control::Auto);

    // Setup the actual logger
    Appender* appender;
    if (gtnh2Packwiz::init::args.count("logfile")) {
        appender = new FileAppender("default", args["logfile"].as<std::string>());
    } else {
        appender = new OstreamAppender("console", &cout);
    }
    appender->setLayout(new gtnh2Packwiz::loggerLayout());
    Category& root = Category::getRoot();
    root.setAppender(appender);
    try { // Set loglevel
        root.setPriority(gtnh2Packwiz::init::getPriority());
    } catch (std::runtime_error& e) {
        root.error(e.what());
        throw;
    }
    root.debug("Logging initialized");
}