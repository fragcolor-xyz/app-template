cmake_minimum_required(VERSION 3.18.1)
project(frag-app)

set(CHAINBLOCKS_ROOT chainblocks CACHE FILEPATH "Path to chainblocks project root")
include(${CHAINBLOCKS_ROOT}/cmake/Root.cmake)

add_subdirectory(fragments)
