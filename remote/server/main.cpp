#include "../commands.hpp"
#include "../router.hpp"
#include <SDL_main.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/asio.hpp>
#include <catch2/catch_session.hpp>
#include <spdlog/spdlog.h>
#include <stdint.h>
#include <stdlib.h>

using boost::archive::binary_iarchive;
using boost::asio::io_context;
using boost::asio::ip::tcp;
using remote::IRoutedCommand;

struct Options {
  uint16_t port = 11331;
};

struct RunTestsHandler : public IRoutedCommand {
  int argc;
  const char **argv;

  RunTestsHandler(int argc, const char **argv) : argc(argc), argv(argv) {}
  void handle(binary_iarchive &ar) {
    remote::commands::RunTests cmd;
    ar >> cmd;

    auto session = Catch::Session();
    session.applyCommandLine(argc, argv);
    session.configData().testsOrTags = cmd.testsOrTags;
    session.run();
  }
};

struct ExitHandler : public IRoutedCommand {
  std::function<void()> onExit;

  ExitHandler(std::function<void()> onExit) : onExit(onExit) {}
  void handle(binary_iarchive &archive) { onExit(); }
};

struct App {
  bool exitRequested{};
  Options options;
  remote::CommandRouter router;

  io_context &ioContext;

  struct Packets {
    remote::commands::Header header;
  };

  // std::vector<>

  App(io_context &ioContext, const Options &options, int argc,
      const char **argv)
      : options(options), ioContext(ioContext) {
    // router.registerCommand<typename TIn, typename TOut, typename TArgs>(const
    // char *key, CommandTraits<TIn, TOut>::Callback &callback)
    // router.registerCommand<RunTestsHandler>("RunTests", argc, argv);
    // router.registerCommand<ExitHandler>("Exit", [&]() {
    //   spdlog::info("Exit requested");
    //   exitRequested = true;
    // });
  }

  void poll() {}

  void run() {
    tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), options.port));
    spdlog::info("Test server running on port {}", options.port);

    while (!exitRequested) {
      tcp::iostream stream;
      acceptor.accept(*stream.rdbuf());
      handleClient(stream);
      ioContext.run();
    }
  }

  void handleClient(tcp::iostream &stream) {
    auto &socket = stream.socket();
    spdlog::info("Connection from {}",
                 socket.remote_endpoint().address().to_string());

    binary_iarchive readArchive(stream);
    while (!exitRequested && !stream.bad()) {
      remote::commands::Header hdr{};

      try {
        readArchive >> hdr;
      } catch (boost::archive::archive_exception &failure) {
        spdlog::warn("Stream interrupted: {}", failure.what());
      }

      router.handle(hdr.message, readArchive);
    }

    spdlog::info("Connection closed");
  }
};

extern "C" {
int SDL_main(int argc, char **argv) {
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
    }
    if (strcmp(arg, "--port") == 0) {
      if (!parseAdditionalArg("--port"))
        break;

      options.port = uint16_t(std::atoi(argv[argOffset]));
    } else {
      spdlog::error("Unrecognized argument: {}", arg);
      argError = true;
    }
  }

  if (argError)
    return 1;

  try {
    io_context ioCtx;

    // Child arguments
    std::vector<std::string> childArgs{argv[0]};
    for (int i = argOffset; i < argc; i++) {
      childArgs.push_back(argv[i]);
    }
    std::vector<const char *> childArgv;
    for (auto &str : childArgs)
      childArgv.push_back(str.c_str());

    App(ioCtx, options, childArgv.size(), childArgv.data()).run();
  } catch (std::exception &ex) {
    spdlog::error("Unhandled: {}", ex.what());
    return 1;
  }

  return 0;
}
}
