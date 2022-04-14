cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0114 NEW)

project(frag-app)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set(CHAINBLOCKS_ROOT chainblocks CACHE FILEPATH "Path to chainblocks project root")
include(${CHAINBLOCKS_ROOT}/cmake/Root.cmake)

add_subdirectory(fragments)
add_subdirectory(remote)
