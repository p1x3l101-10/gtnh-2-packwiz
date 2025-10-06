#define DEFINE_ARGS
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>
#include "buildinfo.hpp"
#include "config.hpp"
#include "gtnh2Packwiz/init.hpp"
#include "license.h"

namespace po = boost::program_options;

boost::program_options::variables_map args;

void gtnh2Packwiz::init::argProcesser(std::pair<int, char**> arg) {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "prints this help message")("version", "print the version info")("buildinfo", "prints the options used to build this program")("license", "print the full license info for this program")("logfile", po::value<std::string>(), "path to log file")("loglevel", po::value<std::string>(), "minimum loglevel to use")("config", po::value<std::string>(), "the config file to use")("pack-version", po::value<std::string>(), "version of pack to assemble");
    try {
        po::store(po::parse_command_line(arg.first, arg.second, desc), args);
    } catch (po::unknown_option& uo) {
        std::cerr << uo.what() << "\n"
                  << desc << std::endl;
        exit(1);
    }
    // Fail fast if nothing is called
    if (args.empty()) {
        std::cerr << desc << std::endl;
        exit(1);
    }
    // Arg options before logging
    if (args.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    }
    if (args.count("version")) {
        std::cout << NAME << " (version: " << VERSION << ")\n"
                  << DESCRIPTION << "\n"
                  << HOMEPAGE_URL << "\n"
                  << "\tCopyright 2025 Scott Blatt, SPDX short identifier: BSD-3-Clause" << std::endl;
        exit(0);
    }
    if (args.count("license")) {
        std::cout << NAME << " (version: " << VERSION << "):\n"
                  << LICENSE_TEXT << std::endl;
        exit(0);
    }
    if (args.count("buildinfo")) {
        std::cout << "Build config: ";
        for (const auto& config : ::configInfo) {
            std::cout << "\n\t" << config.first << "=" << config.second;
        }
        std::cout << std::endl;
        exit(0);
    }
}