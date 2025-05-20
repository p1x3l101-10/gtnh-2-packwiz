#include "gtnh2Packwiz/pack.hpp"
#include <boost/process.hpp>

namespace bp2 = boost::process::v2;
namespace asio = boost::asio;
using std::string;
using std::filesystem::path;
using std::vector;
using std::stringstream;

path derefPath(string bin, string PATH) {
  // Parse PATH into a more usable format
  vector<string> path;
  {
    stringstream ss(PATH);
    string t;
    char del = ':';
    while(std::getline(ss, t, del)) {
      path.push_back(t);
    }
  }

  // Find the file
  class path binary;
  for (const string& test : path) {
    class path candidate = test + bin;
    if (std::filesystem::exists(candidate)) {
      binary = candidate;
      break;
    }
  }
  return binary;
}

void gtnh2Packwiz::pack::extractZip(std::filesystem::path zip, std::string outDir) {
  std::filesystem::path binary = derefPath("unzip", std::getenv("PATH")).string();
  // Setup
  asio::io_context ctx;
  vector<string> args = {
    zip.string(),
    "-d",
    outDir
  };
  args.erase(args.begin());
  bp2::process proc(ctx, binary.string(), args);

  // Exec
  int result = proc.wait();

  // If there are any failures in the child
  if ( result != 0 ) {
    std::cerr << "Dumping cmdline: " << std::endl;
    for (const std::string& substr : args) {
      std::cerr << '\t' << substr << std::endl;
    }
    throw std::runtime_error("Process exited with code: " + std::to_string(result));
  }
}