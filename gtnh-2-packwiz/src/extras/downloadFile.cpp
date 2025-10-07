#include <cstddef>
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
#include <indicators/dynamic_progress.hpp>
#include <indicators/progress_bar.hpp>
#include "config.hpp"

namespace fs = std::filesystem;
using fs::path;
using std::string;
namespace co = curlpp::options;
namespace ct = curlpp::Types;
namespace pb = indicators;
using pb::ProgressBar;
using pb::DynamicProgress;

void gtnh2Packwiz::extras::downloadFile(string url, path destination, int barID, DynamicProgress<ProgressBar>* bars) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.downloadFile");
    if (fs::exists(destination)) {
        logger.debug("Deleting old file...");
        fs::remove(destination);
    }
    try {
        logger.infoStream() << "Downloading URL: '" << url << "'";
        curlpp::Easy request;
        // Create callback
        int percent = 0;
        ct::ProgressFunctionFunctor progressCallback = [&bars, barID, &percent](double dltotal, double dlnow, double ultotal, double ulnow){
            double dlOnePercent = dltotal / 100;
            int dlnowR = std::round(dlnow);
            int dlOnePercentR = std::round(dlOnePercent);
            if (dlnow < dlOnePercent) {
                percent = 0;
            } else if ((dlnowR % dlOnePercentR) != percent) {
                percent++;
                (*bars)[barID].tick();
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