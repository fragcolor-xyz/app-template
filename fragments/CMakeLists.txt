# set(fragments_SOURCES main.cpp)
set(fragments_SOURCES main1.cpp)
# set(fragments_SOURCES main2.cpp)

if(ANDROID)
    add_library(fragments SHARED ${fragments_SOURCES})
else()
    add_executable(fragments ${fragments_SOURCES})
endif()

target_link_libraries(fragments spdlog Boost::algorithm gfx gfx_imgui cbl-static)
# target_link_libraries(fragments spdlog Boost::algorithm SDL2-static SDL2main)
target_bundle_files(fragments PRIVATE bundled entry.edn)

set_target_properties(
  fragments
  PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_GUI_IDENTIFIER "Fragments"
  MACOSX_BUNDLE_BUNDLE_NAME "xyz.fragcolor.fragments"
  MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
)
