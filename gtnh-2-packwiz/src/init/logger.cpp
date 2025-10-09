#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include "gtnh2Packwiz/init.hpp"
#include "gtnh2Packwiz/loggerLayout.hpp"
#ifdef OLD_MAGIC_ENUM
#include <magic_enum.hpp>
#else
#include <magic_enum/magic_enum.hpp>
#endif
#include <rang/rang.hpp>

using rang::setControlMode;
using rang::control;
using std::cout;
using log4cpp::OstreamAppender;
using log4cpp::FileAppender;
using log4cpp::Category;
using log4cpp::Appender;

enum class colorMode {
    on,
    off,
    automatic
};

struct selectedColorsStruct {
    colorMode mode;
    bool selected = false;
};

static selectedColorsStruct selectedColors;

void gtnh2Packwiz::init::logger() {
    // Setup colors
    if (!selectedColors.selected) {
        if (gtnh2Packwiz::init::args.count("colors")) {
            auto candidate = magic_enum::enum_cast<colorMode>(gtnh2Packwiz::init::args["colors"].as<std::string>());
            selectedColors.mode = candidate.value_or(colorMode::automatic);
        } else {
            selectedColors.mode = colorMode::automatic;
        }
        // Setup rang
        switch (selectedColors.mode) {
            case colorMode::on: {
                setControlMode(control::Force); break;
            }
            case colorMode::off: {
                setControlMode(control::Off); break;
            }
            case colorMode::automatic: {
                setControlMode(control::Auto); break;
            }
        }
        selectedColors.selected = true;
    }

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