#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <log4cpp/Category.hh>
#include "gtnh2Packwiz/extras.hpp"

using std::string;
using nlohmann::json;
using std::shared_ptr;

[[noreturn]] void shutdown(bool fatal = false, bool silent = false);

json gtnh2Packwiz::extras::getModVersion(shared_ptr<json> gtnhAssets, string name, string version) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.getUrl");
    for (const auto &modAsset : (*gtnhAssets)["mods"].get<json::array_t>()) {
        if (modAsset["name"] == name) {
            for (const auto &modVersion : modAsset["versions"].get<json::array_t>()) {
                if (modVersion["version_tag"] == version) {
                    return modVersion;
                }
            }
            logger.fatalStream() << "Failed to find version: '" << version << "' for mod: '" << name << "'";
            shutdown(true);
        }
    }
    logger.fatalStream() << "Failed to find mod: '" << name << "'";
    shutdown(true);
}