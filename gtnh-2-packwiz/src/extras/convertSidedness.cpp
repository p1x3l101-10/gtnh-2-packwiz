#include "gtnh2Packwiz/extras.hpp"
#include "config.hpp"
#include <log4cpp/Category.hh>
#ifdef OLD_MAGIC_ENUM
#include <magic_enum.hpp>
#else
#include <magic_enum/magic_enum.hpp>
#endif

using std::string;

[[noreturn]] void shutdown(bool fatal = false, bool silent = false);

enum class gtnhSides {
    BOTH,
    CLIENT,
    SERVER,
    CLIENT_JAVA9,
    SERVER_JAVA9
};
enum class pwSides {
    both,
    client,
    server
};

pwSides convertEnum(gtnhSides side) {
    switch (side) {
        case gtnhSides::CLIENT:
        case gtnhSides::CLIENT_JAVA9: {
            return pwSides::client;
        }
        case gtnhSides::SERVER:
        case gtnhSides::SERVER_JAVA9: {
            return pwSides::server;
        }
        case gtnhSides::BOTH: {
            return pwSides::both;
        }
    }
}

string gtnh2Packwiz::extras::convertSidedness(string side) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.convertSidedness");
    auto oldSide = magic_enum::enum_cast<gtnhSides>(side);
    if (oldSide.has_value()) {
        pwSides newSide = convertEnum(oldSide.value());
        return (magic_enum::enum_name<pwSides>(newSide)).data();
    } else {
        logger.fatal("Bad mod side");
        shutdown(true);
    }
}