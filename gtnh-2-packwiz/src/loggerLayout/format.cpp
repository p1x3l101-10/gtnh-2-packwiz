#include <chrono>
#include <log4cpp/Priority.hh>
#include <log4cpp/threading/PThreads.hh>
#include <sstream>
#include <string>
#include "gtnh2Packwiz/loggerLayout.hpp"

static auto program_start = std::chrono::system_clock::now();
using std::string;

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

  // Create the output string
  out << elapsedStr << priority << " " << thread;
  if (!event.categoryName.empty()) {
    out << " (" << event.categoryName << ")";
  } else {
    out << " ";
  }
  out << ": " << event.message << "\n";

  return out.str();
}