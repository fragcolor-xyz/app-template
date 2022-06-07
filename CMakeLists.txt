cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0114 NEW)

project(frag-app)

# Output to /bin
foreach(CONFIG_NAME ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CONFIG_NAME} CONFIG_NAME)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_NAME} ${CMAKE_BINARY_DIR}/bin)
endforeach()

set(SHARDS_ROOT shards CACHE FILEPATH "Path to shards project root")
include(${SHARDS_ROOT}/cmake/Root.cmake)

# Separate binary directory
add_subdirectory(app)
