#include "gtnh2Packwiz/pack.hpp"
#include <filesystem>
#include <fstream>

std::ofstream gtnh2Packwiz::pack::writeToOut(std::string &fileName) {
  return std::ofstream(getFile(fileName));
}