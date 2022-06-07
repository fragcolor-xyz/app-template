set(app_SOURCES main.cpp)

if(ANDROID)
    add_library(app SHARED ${app_SOURCES})
else()
    add_executable(app ${app_SOURCES})
endif()

target_link_libraries(app spdlog Boost::algorithm gfx gfx_imgui shards-static)
target_bundle_files(app PRIVATE bundled entry.edn)

set_target_properties(
  app
  PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_GUI_IDENTIFIER "App"
  MACOSX_BUNDLE_BUNDLE_NAME "xyz.fragcolor.app"
  MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
)
