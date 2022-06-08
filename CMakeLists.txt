cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0114 NEW)

project(frag-app)

# Output to /bin
string(TOUPPER ${CMAKE_BUILD_TYPE} CONFIG_NAME)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_NAME} ${CMAKE_BINARY_DIR}/bin)
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_NAME}=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_NAME}}")

message("CMAKE_SYSROOT=${CMAKE_SYSROOT}")

set(SHARDS_ROOT shards CACHE FILEPATH "Path to shards project root")
include(${SHARDS_ROOT}/cmake/Root.cmake)

# Separate binary directory
add_subdirectory(app)
