#include "gtnh2Packwiz/extras.hpp"
#include <zip.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <log4cpp/Category.hh>
#include "config.hpp"

namespace fs = std::filesystem;
using fs::path;
using std::string;

class zip {
    private:
        struct zip* za;
        int errorp = 0;
        struct zipFile {
            int index;
            struct zip_stat file;
        };
        std::map<string, zipFile> contents;
        bool contentsInitialized = false;
        void parseContents() {
            struct zip_stat sb;
            for(int i = 0; i < getEntryCount(); i++) {
                if (zip_stat_index(za, i, 0, &sb) == 0){
                    int len = strlen(sb.name);
                    if (sb.name[len - 1] == '/') {
                        // Directory
                    } else {
                        // File
                        zipFile file = {
                            i,
                            sb
                        };
                        contents.insert_or_assign(sb.name, file);
                    }
                }
            }
            contentsInitialized = true;
        };
    public:
        zip(std::string filePath) {
            za = zip_open(filePath.c_str(), 0, &errorp);
            if (za == NULL) {
                throw std::runtime_error("Unable to open zip file!");
            }
        }
        void close() {
            if (zip_close(za) == -1) {
                throw std::runtime_error("Zip close failure!");
            }
        }
        ~zip() {
            try {
                close();
            } catch (...) {} // Destructors dont throw
        }
        int getEntryCount() {
            return zip_get_num_entries(za, 0);
        };
        struct zip* getCArchive() { return za; };
        const std::map<string, zipFile> getContents() {
            if (!contentsInitialized) {
                parseContents();
            }
            return contents;
        }
        void writeFile(std::string zipPath, std::ofstream* realFile) {
            if (!contentsInitialized) {
                parseContents();
            }
            struct zip_file* zf = zip_fopen_index(za, contents[zipPath].index, 0);
            int sum = 0;
            while (sum != contents[zipPath].file.size) {
#               define BUFFERSIZE 100 // Set the buffer size
                char buffer[BUFFERSIZE];
                int len = zip_fread(zf, buffer, BUFFERSIZE);
                if (len < 0) {
                    throw std::runtime_error("Zip reading error!");
                }
                realFile->write(buffer, len);
                sum += len;
            }
            zip_fclose(zf);
        }
};

void safeCreateDirs (path dirName) {
    if (!fs::create_directories(dirName)) {
        if (fs::exists(dirName)) {
            // Folder exists
            return;
        }
        throw std::runtime_error("Failed to create directory");
    }
    return;
}

void gtnh2Packwiz::extras::extractZip(path zipFile, path outDir) {
    log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".extras.extractZip");

    logger.debugStream() << "Opening file: '" << zipFile.string() << "'";
    zip za(zipFile.string());

    for (const auto &file : za.getContents()) {
        logger.debugStream() << "Extracting file: '" << file.first << "'";
        path filePath = outDir.string() + "/" + file.first;
        safeCreateDirs(filePath.parent_path());
        std::ofstream outFile(filePath);
        za.writeFile(file.first, &outFile);
    }
}