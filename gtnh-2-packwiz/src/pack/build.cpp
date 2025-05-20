#include "gtnh2Packwiz/pack.hpp"
#include "gtnh2Packwiz/filecache.hpp"
#include "config.hpp"

using gtnh2Packwiz::filecache;

void gtnh2Packwiz::pack::build() {
  // Get needed files
  filecache indexFile(config->getConfig().repo + "gtnh-assets.json");
  filecache modpackFile(config->getConfig().repo + "gtnh-assets.json");
  filecache versionFile(config->getConfig().repo + "releases/" + packVersion.string() + ".json");
  auto version = versionFile.get(); // Require file
  // Get base zip download
  std::string configVersion = version["config"];
  int index = -1;
  auto assets = indexFile.get();
  auto config = assets["config"]["versions"].array();
  for (int i = 0; i < config.size(); i++) {
    std::string versionCandidate = config.at(i)["version_tag"];
    if (versionCandidate == configVersion) {
      index = i;
      break;
    }
  }
  // Get file
  filecache baseZip(config.at(index)["download_url"]);
  extractZip(baseZip.getFile(), "."); // Require file and extract it
  auto modpack = modpackFile.get(); // Require file
  // Delete ignored files
  for (const auto& file : modpack["client_exclusions"]) {
    if (file != "server.properties") {
      deleteFile(file);
    }
  }
  for (const auto& file : modpack["server_java9_exclusions"]) {
    if (file != "server.properties") {
      deleteFile(file);
    }
  }
  deleteFile("startserver-java9.bat");
  deleteFile("startserver-java9.sh");
}