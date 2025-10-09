#include <string>
#define DEFINE_ARGS
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>
#ifdef OLD_MAGIC_ENUM
#include <magic_enum.hpp>
#else
#include <magic_enum/magic_enum.hpp>
#endif
#include "buildinfo.hpp"
#include "config.hpp"
#include "gtnh2Packwiz/init.hpp"
#include "license.h"

namespace po = boost::program_options;

boost::program_options::variables_map args;

extern void shutdown(bool fatal = false, bool silent = true);

void gtnh2Packwiz::init::argProcesser(std::pair<int, char**> arg) {
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help", "prints this help message")
        ("version", "print the version info")
        ("buildinfo", "prints the options used to build this program")
        ("license", "print the full license info for this program")
        ("colors", po::value<std::string>(), "color mode (on, off, or automatic(default))")
        ("clear-cache", "clear the cache on completion")
        ("logfile", po::value<std::string>(), "path to log file")
        ("loglevel", po::value<std::string>(), "minimum loglevel to use")
        ("config", po::value<std::string>(), "the config file to use")
        ("pack-version", po::value<std::string>(), "version of pack to assemble")
    ;
    // clang-format on
    try {
        po::store(po::parse_command_line(arg.first, arg.second, desc), args);
    } catch (po::unknown_option& uo) {
        std::cerr << uo.what() << "\n"
                  << desc << std::endl;
        shutdown(1);
    }
    // Fail fast if nothing is called
    if (args.empty()) {
        std::cerr << desc << std::endl;
        shutdown(1);
    }
    // Arg options before logging
    if (args.count("help")) {
        std::cout << desc << std::endl;
        shutdown(0);
    }
    if (args.count("version")) {
        std::cout << NAME << " (version: " << VERSION << ")\n"
                  << DESCRIPTION << "\n"
                  << HOMEPAGE_URL << "\n"
                  << "\tCopyright 2025 Scott Blatt, SPDX short identifier: BSD-3-Clause" << std::endl;
        shutdown(0);
    }
    if (args.count("license")) {
        std::cout << NAME << " (version: " << VERSION << "):\n"
                  << LICENSE_TEXT << std::endl;
        shutdown(0);
    }
    if (args.count("buildinfo")) {
        std::cout << "Build config: ";
        for (const auto& config : ::configInfo) {
            std::cout << "\n\t" << config.first << "=" << config.second;
        }
        std::cout << std::endl;
        shutdown(0);
    }
}