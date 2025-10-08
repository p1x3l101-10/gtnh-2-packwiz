#include "gtnh2Packwiz/extras.hpp"
#include "gtnh2Packwiz/poolManager.hpp"
#include <boost/process.hpp>
#include <boost/process/v2/environment.hpp>
#include <filesystem>
#include <string>
#include <log4cpp/Category.hh>
#include <vector>

namespace fs = std::filesystem;
using fs::path;
using std::string;
using boost::process::environment::find_executable;
using boost::process::process;
using std::vector;

extern gtnh2Packwiz::poolManager pool;

[[noreturn]] void shutdown(bool fatal = false, bool silent = false);

static bool signifyError = false;

void gtnh2Packwiz::extras::callSignify(path file, path signature, path key) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.generatePWHash");
    if (!signifyError) {
        auto signifyBinCandidate = find_executable("signify");
        if (signifyBinCandidate.empty()) {
            logger.error("Unable to find signify! Please install signify if you want to sign files.");
        } else {
            path signifyBin = signifyBinCandidate.string(); // Cast to STL path via string to use STL functions
            // Generate args
            vector<string> args = {
                "-S",
                    "-s", key,
                    "-m", file,
                    "-x", signature
            };

            auto tp = pool.getPool().lock();
            process signifyProc(
                tp->get_executor(),
                signifyBin.string(),
                args
            );
            logger.debugStream() << "Signed file: '" << file.string() << "'";
            return; // Needed to avoid triggering bad log messages
        }
    }
    // If you make it here, the conditional was skipped
    logger.warnStream() << "Failed to sign file: '" << file.string() << "'";
}