#include "gtnh2Packwiz/configFile.hpp"
#include <filesystem>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>
#include "config.hpp"

#define GET_VALUE(PATH, TYPE) cfg.at_path(PATH).value_or(sys.at_path(PATH).ref<TYPE>())

namespace fs = std::filesystem;
using std::string;

gtnh2Packwiz::configFile::configFile(std::string configPath) {
  fs::path rawConfigFile = configPath;
  fs::path configFile;
  fs::path sysConfigFile = SYSTEM_CONFIG_PATH;
  if (rawConfigFile.c_str()[0] == '~') { // Resolve home references that the shell failed to handle
    logger.debug("'~' in file path assumed to be refering to home, correcting path...");
    std::string filePath = rawConfigFile.string();
    filePath.erase(0, 1);
    std::string homeDir = std::getenv("HOME");
    rawConfigFile = homeDir + filePath;
  }
  if (!rawConfigFile.is_absolute()) {
    logger.debug("File path is not absolute, correcting...");
    configFile = std::filesystem::absolute(rawConfigFile);
  } else {
    configFile = rawConfigFile;
  }
  if (!sysConfigFile.is_absolute()) {
    logger.warn("System file path is not absolute, please check your build system. Proceding anyway...");
    sysConfigFile = std::filesystem::absolute(sysConfigFile);
  }
  try {
    logger.info("Loading system config file: " + sysConfigFile.string());
    toml::table sys = toml::parse_file(sysConfigFile.string());
    toml::table cfg;
    if (std::filesystem::exists(configFile)) {
      logger.info("Loading admin config file: " + configFile.string());
      cfg = toml::parse_file(configFile.string());
    }
    logger.debug("Setting internal configuration");
    config = {
        GET_VALUE("config.repo", string),
        GET_VALUE("config.configRepo", string),
        GET_VALUE("config.cfApiKey", string),
        GET_VALUE("config.generateRemoteHashes", bool),
        GET_VALUE("config.outPath", string)};
  } catch (toml::parse_error& e) {
    logger.fatalStream() << "Toml parsing error!" << e.description();
    exit(1);
  }
}