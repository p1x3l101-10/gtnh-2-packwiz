#include <chrono>
#include <cstddef>
#include <ctime>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Option.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Types.hpp>
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
namespace ct = curlpp::Types;
namespace chrono = std::chrono;

void gtnh2Packwiz::extras::downloadFile(string url, path destination) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.downloadFile");
    if (fs::exists(destination)) {
        // Only delete file when it is older than 12 hours
        auto now = chrono::file_clock::now();
        auto fileAge = fs::last_write_time(destination);
        auto age = now - fileAge;
        if (age > chrono::hours(12)) {
            logger.debug("Deleting stale file...");
            fs::remove(destination);
        } else {
            logger.info("Using cached file");
            return;
        }
    }
    try {
        logger.infoStream() << "Downloading URL: '" << url << "'";
        curlpp::Easy request;
        // Create callback
        log4cpp::Category& callbacklogger = log4cpp::Category::getInstance(NAME ".extras.downloadFile.callback");
        auto t0 = std::chrono::high_resolution_clock::now();
        auto tL0 = std::chrono::high_resolution_clock::now();
        auto tf = std::chrono::high_resolution_clock::now();
        auto deltaT = std::chrono::milliseconds(500);
        auto longDeltaT = std::chrono::seconds(30);
        int longOffence = 0;
        ct::ProgressFunctionFunctor progressCallback = [&logger = callbacklogger, &t0, &tL0, &tf, &longOffence, deltaT, longDeltaT, destination](double dltotal, double dlnow, double ultotal, double ulnow){
            tf = std::chrono::high_resolution_clock::now();
            if ((tf - t0) >= deltaT) {
                logger.debugStream() << "Downloaded: `" << humanReadableBytes(dlnow) << "` out of: `" << humanReadableBytes(dltotal) << "`"; 
                t0 = std::chrono::high_resolution_clock::now();
            }
            if ((tf - tL0) >= longDeltaT) {
                if (longOffence == 0) {
                    logger.warnStream() << "Download is taking a long time, offending file: '" << destination.string() << "'";
                } else {
                    logger.errorStream() << "Download is taking a very long time, is something wrong?";
                }
                longOffence++;
            }
            return 0; // 0 to continue, 1 to abort
        };
        // Write to file
        std::ofstream outFile(destination, std::ios::out);
        // Create the request
        request.setOpt(new co::Url(url));
        request.setOpt(new co::WriteStream(&outFile));
        request.setOpt(new co::NoProgress(false));
        request.setOpt(new co::ProgressFunction(progressCallback));
        request.setOpt(new co::FollowLocation(true)); // Fucking github and its countless redirections
        request.perform();
        // Mark as completed
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