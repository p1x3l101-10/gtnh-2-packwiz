#include <chrono>
#include <log4cpp/Priority.hh>
#include <log4cpp/threading/PThreads.hh>
#include <sstream>
#include <string>
#include <rang/rang.hpp>
#include "gtnh2Packwiz/loggerLayout.hpp"

static auto program_start = std::chrono::system_clock::now();
using std::string;
using rang::style;
using rang::control;
using rang::fg;
using rang::bg;
using log4cpp::Priority;

std::string gtnh2Packwiz::loggerLayout::format(const log4cpp::LoggingEvent& event) {
    std::ostringstream out;

    // Get log time
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = now - program_start;
    string elapsedStr = std::to_string(elapsed.count());
    elapsedStr.resize(11, ' ');

    // Loglevel
    string priority = "[" + log4cpp::Priority::getPriorityName(event.priority) + "]";
    priority.resize(7, ' ');

    // Thread PID
    string thread = "<" + log4cpp::threading::getThreadId() + ">";
    thread.resize(14, ' ');

    // Colors for the output
    std::ostringstream color;
    switch (event.priority) {
      //case log4cpp::Priority::FATAL: // Same
        case log4cpp::Priority::EMERG: {
            color << style::blink << fg::red << bg::gray << style::underline << style::bold;
            break;
        }
        case log4cpp::Priority::ALERT: {
            color << fg::red << bg::gray << style::bold << style::underline;
            break;
        }
        case log4cpp::Priority::CRIT: {
            color << fg::red << style::bold;
            break;
        }
        case log4cpp::Priority::ERROR: {
            color << fg::red << style::bold;
            break;
        }
        case log4cpp::Priority::WARN: {
            color << fg::yellow;
            break;
        }
        case log4cpp::Priority::NOTICE: {
            color << fg::blue;
            break;
        }
        case log4cpp::Priority::INFO: {
            break; // No color changes needed
        }
        case log4cpp::Priority::DEBUG: {
            color << style::dim << fg::gray;
            break;
        }
        case log4cpp::Priority::NOTSET: {
            break;
        }
    }

    // Create the output string
    out << color.str() << elapsedStr << priority << " " << thread;
    if (!event.categoryName.empty()) {
        out << " (" << event.categoryName << ")";
    } else {
        out << " ";
    }
    out << ": " << event.message << "\n";

    // Reset colors
    out << style::reset << fg::reset << bg::reset;

    return out.str();
}