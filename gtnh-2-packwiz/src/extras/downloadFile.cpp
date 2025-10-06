#include "gtnh2Packwiz/extras.hpp"
#include <cstddef>
#include <cstdio>
#include <curlpp/Exception.hpp>
#include <curlpp/Option.hpp>
#include <log4cpp/Category.hh>
#include "config.hpp"
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;
using std::string;
using fs::path;

// Create a file object so that I avoid bugs
class fileOb {
    private:
        FILE* file = nullptr;
    public:
        fileOb(const std::string filename, const std::string writeMode) {
            file = fopen(filename.c_str(), writeMode.c_str()); // SEGFAULTS HERE
        }
        ~fileOb() {
            fclose(file);
        }
        FILE* getFile() {
            return file;
        }
};

// Writing callback
static size_t writeFileCallback(char* ptr, size_t size, size_t nmemb, void *f) {
    FILE* file = (FILE*)f;
	return fwrite(ptr, size, nmemb, file);
}
void gtnh2Packwiz::extras::downloadFile(string url, path destination) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.downloadFile");
    try {
        logger.infoStream() << "Downloading URL: '" << url << "'";
        curlpp::Easy request;
        // Set the callback to a file writer
        curlpp::options::WriteFunctionCurlFunction writer(writeFileCallback);
        fileOb file(destination, "wb");
        curlpp::OptionTrait<void*, CURLOPT_WRITEDATA> data(file.getFile());
        // Create the request
        request.setOpt(writer);
        request.setOpt(data);
        request.setOpt(new curlpp::options::Url(url));
        request.perform();
        return; // Done now
    } catch (curlpp::LogicError &e) {
        logger.fatalStream() << e.what();
        throw;
    } catch (curlpp::RuntimeError &e) {
        logger.fatalStream() << e.what();
        throw;
    } catch (std::runtime_error &e) {
        logger.emergStream() << e.what();
        throw;
    }
}