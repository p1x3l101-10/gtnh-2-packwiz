#pragma once
#include <log4cpp/Layout.hh>
#include <log4cpp/LoggingEvent.hh>
#include <string>

namespace gtnh2Packwiz {
  class loggerLayout : public log4cpp::Layout {
      virtual ~loggerLayout() {};
      std::string format(const log4cpp::LoggingEvent& event);
  };
} // namespace gtnh2Packwiz