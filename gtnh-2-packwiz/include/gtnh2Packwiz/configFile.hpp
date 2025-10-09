#pragma once
#include <log4cpp/Category.hh>
#include <string>
#include "config.hpp"

namespace gtnh2Packwiz {
    class configFile {
        protected:
            struct configStruct {
                    std::string repo;
                    std::string configRepo;
                    std::string cfApiKey;
                    bool generateRemoteHashes;
                    std::string outPath;
            };
#if USING_UNSUP == 1
            struct unsupStruct {
                std::string targetURL;
                bool enableSigning;
                std::string publicKey;
                std::string privateKeyPath;
                bool noGui;
                std::string configPath;
            };
#endif
        private:
            log4cpp::Category& logger = log4cpp::Category::getInstance(NAME ".configFile");
            configStruct config;
#if USING_UNSUP == 1
            unsupStruct unsup;
#endif
        public:
            configFile(std::string filePath);
            const configStruct getConfig() const;
#if USING_UNSUP == 1
            const unsupStruct getUnsupConfig() const;
#endif
    };
} // namespace gtnh2Packwiz