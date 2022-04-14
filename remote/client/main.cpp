
#include "../commands.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <stdint.h>
#include <stdlib.h>

using boost::archive::binary_iarchive;
using boost::archive::binary_oarchive;
using boost::asio::io_context;
using boost::asio::ip::address_v4;
using boost::asio::ip::tcp;
using boost::system::error_code;

struct Options {
  // Server to connect to
  address_v4 serverAddress = address_v4::loopback();
  // Port the server is listening on
  uint16_t serverPort = 11331;
};

int main(int argc, char **argv) {
  Options options{};

  int argOffset;
  bool endArgs = false;
  bool argError = false;

  auto parseAdditionalArg = [&](const char *key) {
    if (++argOffset >= argc) {
      spdlog::error("{} requires an argument", key);
      argError = true;
      return false;
    }
    return true;
  };

  for (argOffset = 1; !endArgs && argOffset < argc; argOffset++) {
    const char *arg = argv[argOffset];
    if (strcmp(arg, "--") == 0) {
      endArgs = true;
    } else if (strcmp(arg, "--server") == 0) {
      if (!parseAdditionalArg("--server"))
        break;

      error_code err;
      const char *addrStr = argv[argOffset];
      options.serverAddress = address_v4::from_string(addrStr, err);
      if (err.failed()) {
        spdlog::error("Failed to parse server address \"{}\": {}", addrStr,
                      err.what());
        argError = true;
      }
    } else if (strcmp(arg, "--port") == 0) {
      if (!parseAdditionalArg("--port"))
        break;

      options.serverPort = uint16_t(std::atoi(argv[argOffset]));
    } else {
      spdlog::error("Unrecognized argument: {}", arg);
      argError = true;
    }
  }

  if (argError)
    return 1;

  if (options.serverAddress.is_unspecified()) {
    spdlog::error("No server address specified");
    return 1;
  }

  // Child arguments
  std::vector<std::string> childArgs{argv[0]};
  for (int i = argOffset; i < argc; i++) {
    childArgs.push_back(argv[i]);
  }

  io_context ioCtx;

  try {
    tcp::socket sock(ioCtx);

    spdlog::info("Connecting to test server {}:{}",
                 options.serverAddress.to_string(), options.serverPort);
    tcp::iostream stream;
    stream.connect(tcp::endpoint(options.serverAddress, options.serverPort));

    binary_oarchive writeArchive(stream);
    writeArchive << remote::commands::Header{.message = "RunTests"};
    writeArchive << remote::commands::RunTests{};
    writeArchive << remote::commands::Header{.message = "Exit"};
    writeArchive << remote::commands::Exit{};

  } catch (std::exception &ex) {
    spdlog::error("Unhandled: {}", ex.what());
    return 1;
  }

  return 0;
}
