#pragma once
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace remote {
namespace commands {
struct Header {
  std::string message;
  template <typename Ar> void serialize(Ar &ar, const unsigned int version) {
    ar &message;
  }
};

struct RunTests {
  std::vector<std::string> testsOrTags;
  template <typename Ar> void serialize(Ar &ar, const unsigned int version) {
    ar &testsOrTags;
  }
};

struct LogData {
  std::vector<std::string> lines;
  template <typename Ar> void serialize(Ar &ar, const unsigned int version) {
    ar &lines;
  }
};

struct Exit {
  template <typename Ar> void serialize(Ar &ar, const unsigned int version) {}
};
} // namespace commands
} // namespace remote