#pragma once

#include <filesystem>
#include <string>
#include <indicators/dynamic_progress.hpp>
#include <indicators/progress_bar.hpp>
#include "config.hpp"

namespace gtnh2Packwiz::extras {
    void downloadFile(std::string url, std::filesystem::path destination, int barID, indicators::DynamicProgress<indicators::ProgressBar>* bars);
    void extractZip(std::filesystem::path zip, std::filesystem::path outDir, int barID, indicators::DynamicProgress<indicators::ProgressBar>* bars);
} // namespace gtnh2Packwiz::extras