#define INIT_VARS
#include "gtnh2Packwiz/filecache.hpp"
#include "config.hpp"
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <filesystem>

using std::string;
namespace fs = std::filesystem;
typedef unsigned char byte;

bool olderThanMinutes(int minutes, fs::file_time_type mTime) {
  auto sctp = time_point_cast<std::chrono::system_clock::duration>(mTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

  auto now = std::chrono::system_clock::now();
  auto age = duration_cast<std::chrono::minutes>(now - sctp);

  return age > std::chrono::minutes(minutes);
}

gtnh2Packwiz::filecache::filecache(string uri) {
  logger.debug("Hashing URI");
  CryptoPP::SHA1 hash;
  CryptoPP::StringSource ss(uri, true,
    new CryptoPP::HashFilter(hash,
      new CryptoPP::HexEncoder(
        new CryptoPP::StringSink(digest), false
      )
    )
  );
  meta.uri = uri;
  // Ensure that the cache path exists
  std::filesystem::create_directories(CACHE);
  // Check if file is downloaded
  fs::path file = CACHE"/" + digest;
  if (fs::exists(file)) {
    meta.freshFile = false;
    meta.fetchTime = fs::last_write_time(file);
    if (olderThanMinutes(60, meta.fetchTime)) {
      downloadFile(); // Redownload if file is too old
    } else {
      fileReady = true; // Mark file as good if present and not stale
    }
  } else {
    downloadFile(); // Download if not exists
  }
}