#include <cstddef>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Option.hpp>
#include <curlpp/Options.hpp>
#include <filesystem>
#include <fstream>
#include <log4cpp/Category.hh>
#include <stdexcept>
#include <string>
#include "config.hpp"
#include "gtnh2Packwiz/extras.hpp"

namespace fs = std::filesystem;
using fs::path;
using std::string;
namespace co = curlpp::options;

extern gtnh2Packwiz::poolManager pool;

void gtnh2Packwiz::extras::downloadFile(string url, path destination) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.downloadFile");
    if (fs::exists(destination)) {
        logger.debug("Deleting old file...");
        fs::remove(destination);
    }
    try {
        logger.infoStream() << "Downloading URL: '" << url << "'";
        curlpp::Easy request;
        // Write to file
        std::ofstream outFile(destination);
        // Create the request
        request.setOpt(new co::Url(url));
        request.setOpt(new co::WriteStream(&outFile));
        request.perform();
        return; // Done now
    } catch (curlpp::LogicError& e) {
        logger.fatalStream() << e.what();
        throw;
    } catch (curlpp::RuntimeError& e) {
        logger.fatalStream() << e.what();
        throw;
    } catch (std::runtime_error& e) {
        logger.emergStream() << e.what();
        throw;
    }
}