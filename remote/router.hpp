#pragma once
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <functional>
#include <map>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <string>

namespace remote {
using boost::archive::binary_iarchive;
using boost::archive::binary_oarchive;
using std::map;
using std::runtime_error;
using std::string;

struct IRoutedCommand {
  virtual ~IRoutedCommand() = default;
  virtual void handle(binary_iarchive &archive) = 0;
};

struct ICommandDataContainer {
  virtual ~ICommandDataContainer() = default;
  virtual void read(binary_iarchive &ar) = 0;
  virtual void write(binary_oarchive &ar) = 0;
};
using CommandDataContainerPtr = std::shared_ptr<ICommandDataContainer>;

struct ICommandHandler {
  virtual ~ICommandHandler() = default;
  virtual CommandDataContainerPtr inputArgs() = 0;
  virtual CommandDataContainerPtr handle(CommandDataContainerPtr input) = 0;
};

template <typename TIn, typename TOut> struct CommandTraits {
  typedef std::function<void(const TIn &in, TOut &out)> Callback;
  struct InputData : public ICommandDataContainer {
    TIn data;
    void read(binary_iarchive &ar) { ar &data; }
    void write(binary_oarchive &ar) { ar &data; }
  };

  struct OutputData : public ICommandDataContainer {
    TIn data;
    void read(binary_iarchive &ar) { ar &data; }
    void write(binary_oarchive &ar) { ar &data; }
  };

  struct Handler : public ICommandHandler {
    CommandDataContainerPtr inputArgs() { return std::make_shared<InputData>(); }
    CommandDataContainerPtr handle(CommandDataContainerPtr input) {}
  };
};

struct CommandRouter {
private:
  map<string, std::shared_ptr<ICommandHandler>> handlers;

public:
  template <typename TIn, typename TOut, typename... TArgs>
  void registerCommand(const char *key,
                       CommandTraits<TIn, TOut>::Callback &callback) {
    auto routedCommand = std::make_shared<RoutedCommand>(callback);
    return (T &)*handlers.insert_or_assign(key, std::make_shared<T>(args...));
  }

  void handle(const std::string &commandKey, binary_iarchive &ar) {
    auto it = handlers.find(commandKey);
    if (it == handlers.end()) {
      throw runtime_error(fmt::format("Unrecognized command: {}", commandKey));
    }

    auto &command = it->second;
    // command->handle(ar);
  }
};
} // namespace remote