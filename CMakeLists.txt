cmake_minimum_required(VERSION 3.31.6)
# Import metadata
include(cmake/fetchToml.cmake)
fetchTomlInit("./metadata.toml")
fetchToml(pname CMAKE_PROJECT_NAME)
fetchToml(version CMAKE_PROJECT_VERSION)
fetchToml(description CMAKE_PROJECT_DESCRIPTION)
fetchToml(homepage CMAKE_PROJECT_HOMEPAGE_URL)

project(
    ${CMAKE_PROJECT_NAME}
    DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION}
    HOMEPAGE_URL ${CMAKE_PROJECT_HOMEPAGE_URL}
    LANGUAGES CXX
    VERSION ${CMAKE_PROJECT_VERSION}
)
if(NOT LOCALSTATEDIR)
    set(LOCALSTATEDIR "/tmp")
endif()
include(cmake-libs/all)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# Import libs
find_package(Boost 1.89.0 REQUIRED COMPONENTS program_options process)
find_package(magic_enum 0.9.5 REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(PkgConfig 1.8.1 REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus>=3.4.0)
pkg_check_modules(log4cpp REQUIRED IMPORTED_TARGET log4cpp>=1.1)
pkg_check_modules(cryptopp REQUIRED IMPORTED_TARGET libcryptopp>=8.9)
pkg_check_modules(libzip REQUIRED IMPORTED_TARGET libzip>=1.11.4)
if(BAD_CURLPP_VERSION_FORMAT)
    pkg_check_modules(curlpp REQUIRED IMPORTED_TARGET curlpp)
else()
    pkg_check_modules(curlpp REQUIRED IMPORTED_TARGET curlpp>=0.8.1)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_INSTALL_PREFIX ".")
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
# Set paths
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(SYSTEM_CONFIG_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/${CMAKE_PROJECT_NAME}/config.toml")
    set(DEFAULT_ADMIN_CONFIG_PATH "${CMAKE_INSTALL_FULL_SYSCONFDIR}/${CMAKE_PROJECT_NAME}/config.toml")
else()
    set(SYSTEM_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/defaultConfigs.toml")
    set(DEFAULT_ADMIN_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/config.toml")
endif()
# Threads
set(THREAD_POOL_MAX_THREADS 4)

build(
    HEADER_LIBRARY indicators
)

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
        PkgConfig::libzip
        indicators
    EMBED_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    INSTALL_TARGET
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/defaultConfigs.toml"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}/config.toml"
)