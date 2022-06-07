cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0114 NEW)

project(frag-app)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set(SHARDS_ROOT shards CACHE FILEPATH "Path to shards project root")
include(${SHARDS_ROOT}/cmake/Root.cmake)

add_subdirectory(app)
