set(app_SOURCES main.cpp)

if(ANDROID)
  add_library(libApp SHARED ${app_SOURCES})
else()
  add_library(libApp ${app_SOURCES})
endif()
set_target_properties(libApp PROPERTIES OUTPUT_NAME_${CONFIG_NAME} "App")

target_link_libraries(libApp spdlog Boost::algorithm gfx gfx_imgui shards-static)
target_bundle_files(libApp PRIVATE bundled entry.edn)

if(ANDROID)
  include(Android)
  file(GLOB_RECURSE JAVA_SOURCES "android/src/*")
  file(GLOB_RECURSE SDL_JAVA_SOURCES "${SHARDS_DIR}/deps/SDL/android-project/app/src/main/java/*")
  add_android_package(
      NAME app
      SOURCES ${JAVA_SOURCES} ${SDL_JAVA_SOURCES}
      MANIFEST android/AndroidManifest.xml
      RES_PATH android/res
      LIB_TARGETS libApp
  )
else()
  add_executable(app dummy.cpp)
  target_link_libraries(app libApp)

  set_target_properties(
    app
    PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "App"
    MACOSX_BUNDLE_BUNDLE_NAME "xyz.fragcolor.app"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
  )
endif()