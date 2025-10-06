#include "config.hpp"
#include "gtnh2Packwiz/init.hpp"
#ifdef OLD_MAGIC_ENUM
#include <magic_enum.hpp>
#else
#include <magic_enum/magic_enum.hpp>
#endif
#include <stdexcept>

enum class PriorityLevelMirror { // Magic enum can understand this
    EMERG = 0,
    FATAL = 1,
    ALERT = 10,
    CRIT = 20,
    ERROR = 30,
    WARN = 40,
    NOTICE = 50,
    INFO = 60,
    DEBUG = 70,
    NOTSET = 80
};

log4cpp::Priority::PriorityLevel gtnh2Packwiz::init::getPriority() {
    if (gtnh2Packwiz::init::args.count("loglevel")) {
        auto loglevel = gtnh2Packwiz::init::args["loglevel"].as<std::string>();
        auto newLog = magic_enum::enum_cast<PriorityLevelMirror>(loglevel);
        if (newLog.has_value()) {
            // Weird int math because magic_enum seems to not like values over 200 or so
            int log = static_cast<int>(newLog.value());
            if (log == 1) {
                log--;
            }
            log = log * 10;
            return static_cast<log4cpp::Priority::PriorityLevel>(log);
        } else {
            throw std::runtime_error("Priority \"" + loglevel + "\" does not exist!");
        }
    } else {
        return DEFAULT_LOGLEVEL;
    }
}