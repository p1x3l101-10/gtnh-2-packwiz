#pragma once
#include <log4cpp/Category.hh>
#include <string>
#include <string_view>

constexpr std::string extractFileBase(std::string_view path) {
  size_t slash = path.find_last_of("/\\");
  size_t dot = path.find_last_of('.');
  if (slash == std::string_view::npos)
    slash = -1;
  if (dot == std::string_view::npos || dot < slash)
    dot = path.length();
  const std::string out = static_cast<std::string>(path.substr(slash + 1, dot - slash - 1));
  return out;
}

#define CURRENT_FILE_BASENAME (::extractFileBase(__FILE__)) // Macro friendly

#define createLogger(VAR_NAME) log4cpp::Category& VAR_NAME = log4cpp::Category::getInstance(this->logger.getName() + "." + CURRENT_FILE_BASENAME) // will return a named logger