cmake_minimum_required(VERSION 3.31.6)
# Autoversion
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE "^v" "" GIT_TAG "${GIT_TAG}")
if(GIT_TAG)
    set(CMAKE_PROJECT_VERSION ${GIT_TAG})
else()
    set(CMAKE_PROJECT_VERSION "0.0.0")  # Fallback if no tag is found
endif()

project(
    "gtnh-2-packwiz"
    DESCRIPTION "Processes the GTNH build scripts to create a packwiz release"
    HOMEPAGE_URL "https://github.com/p1x3l101-10/gtnh-2-packwiz"
    LANGUAGES CXX
    VERSION ${CMAKE_PROJECT_VERSION}
)
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

find_package(Boost 1.87.0 REQUIRED COMPONENTS program_options)
find_package(magic_enum 0.9.5 REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(PkgConfig 1.8.1 REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus>=3.4.0)
pkg_check_modules(log4cpp REQUIRED IMPORTED_TARGET log4cpp>=1.1)
pkg_check_modules(cryptopp REQUIRED IMPORTED_TARGET libcryptopp>=8.9)
if(BAD_CURLPP_VERSION_FORMAT)
    pkg_check_modules(curlpp REQUIRED IMPORTED_TARGET curlpp)
else()
    pkg_check_modules(curlpp REQUIRED IMPORTED_TARGET curlpp>=0.8.1)
endif()

include(GNUInstallDirs)
if(PREFIXED_BUILD) # fix the build for things like homebrew
    set(CMAKE_INSTALL_FULL_SYSCONFDIR "${CMAKE_INSTALL_SYSCONFDIR}")
endif()
if(NOT DEFAULT_LOGLEVEL)
    if(("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") OR ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo"))
        set(DEFAULT_LOGLEVEL "DEBUG")
    else()
        set(DEFAULT_LOGLEVEL "INFO")
    endif()
endif()
build(
    BINARY ${CMAKE_PROJECT_NAME}
    LIBRARIES
        PkgConfig::tomlplusplus
        PkgConfig::log4cpp
        PkgConfig::cryptopp
        PkgConfig::curlpp
        Boost::program_options
        magic_enum::magic_enum
        nlohmann_json::nlohmann_json
    EMBED_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    INSTALL_TARGET
    PUBLIC_COMPILE_OPTIONS "-fexperimental-library"
)