#include "gtnh2Packwiz/extras.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <chrono>
#include <log4cpp/Category.hh>
#include <thread>

namespace fs = std::filesystem;
namespace chrono = std::chrono;
using fs::path;
using std::string;
using nlohmann::json;
using std::ifstream;

// A safe version that can handle github rate limits for the api
// Can only be used for api requests, otherwise fails
void gtnh2Packwiz::extras::githubSafeDlFile(string url, path destination, bool debugDownload, expirationConditions expirationConditions) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.githubSafeDlFile");
    bool success = false;
    bool first = true;
    int attempts = 1;
    while (!success) {
        if (first) {
            first = false;
            downloadFile(url, destination, debugDownload, expirationConditions);
        } else {
            // Force a redownload
            logger.infoStream() << "Download attempt: " << attempts;
            downloadFile(url, destination, debugDownload, {false});
        }
        json responce;
        {
            ifstream responceFile(destination);
            responceFile >> responce;
        }
        if ((responce.contains("message")) && (responce["message"].get<json::string_t>().contains("API rate limit exceeded"))) {
            logger.warn("We have hit the github rate limit, letting it cool off...");
            std::this_thread::sleep_for(chrono::seconds(5 * attempts));
            attempts++;
        } else {
            break;
        }
    }
}