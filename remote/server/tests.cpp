#include <catch2/catch_test_macros.hpp>

TEST_CASE("Succeed") { CHECK(true); }

TEST_CASE("Failed") {
  CHECK(false);
}