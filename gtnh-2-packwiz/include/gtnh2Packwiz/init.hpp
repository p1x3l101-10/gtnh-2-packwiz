#pragma once
#include <utility>
#include <boost/program_options/variables_map.hpp>
#include <log4cpp/Priority.hh>


namespace gtnh2Packwiz::init {
  #ifdef DEFINE_ARGS
  boost::program_options::variables_map args;
  #else
  extern boost::program_options::variables_map args;
  #endif
  log4cpp::Priority::PriorityLevel getPriority();
  void argProcesser(std::pair<int,char**> args);
  void logger();
}