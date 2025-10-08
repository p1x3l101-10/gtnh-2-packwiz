#include "gtnh2Packwiz/extras.hpp"
#include "config.hpp"
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <sstream>
#include <stdexcept>
#include <fstream>
#ifdef OLD_MAGIC_ENUM
#include <magic_enum.hpp>
#else
#include <magic_enum/magic_enum.hpp>
#endif

namespace fs = std::filesystem;
using std::string;
using fs::path;
using magic_enum::enum_cast;
using std::ifstream;
using std::stringstream;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::HashFilter;

enum class knownHashFunctions {
    sha256
};

// Ensure that the config.hpp hash format is confined to the known hash functions
// NOTE: if you are reusing this function for other purposes, you can (and should) remove this check along with the whole config.hpp thing
// That is only there for this project
static_assert((enum_cast<knownHashFunctions>(PACKWIZ_HASH_FORMAT)).has_value(), "PACKWIZ_HASH_FORMAT is not one of the supported values!");
// End tests

namespace helpers {
    string sha256(path filePath) {
        CryptoPP::SHA256 hash;
        stringstream buffer;
        {
            // Load file into buffer it will be closed after use due to scoping
            ifstream file(filePath);
            buffer << file.rdbuf();
        }
        string digest;
        // CryptoPP pipelines intentionally look like this
        // Objects are closer to functions in this case (very weird)
        StringSource(buffer.str(), true, new HashFilter(hash, new StringSink(digest)));
        return digest;
    }
}

const string gtnh2Packwiz::extras::generatePWHash(path file, string pwHashFormat) {
    auto hashCandidate = enum_cast<knownHashFunctions>(pwHashFormat);
    if (hashCandidate.has_value()) {
        switch (hashCandidate.value()) {
            case knownHashFunctions::sha256: return helpers::sha256(file);
        }
    } else {
        throw std::runtime_error("Invalid hash format"); // Should never be reached in this project as there is an assert to ensure that. Only useful when copied
    }
}