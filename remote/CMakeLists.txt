set(BASE_TARGET_NAME remote)
set(SHARED_TARGET "${BASE_TARGET_NAME}_shared")
set(SERVER_TARGET "${BASE_TARGET_NAME}_server")
set(CLIENT_TARGET "${BASE_TARGET_NAME}_client")

add_library(${SHARED_TARGET} INTERFACE)
target_link_libraries(${SHARED_TARGET} INTERFACE Boost::asio Boost::serialization)

if(WIN32)
  # 0x0A00 = Windows 10
  target_compile_definitions(${SHARED_TARGET} INTERFACE WINVER=0x0A00 _WIN32_WINNT=0x0A00)
endif()

# The clients runs on a desktop/CI machine to receive the test results
add_executable(${CLIENT_TARGET} client/main.cpp)
target_link_libraries(${CLIENT_TARGET} ${SHARED_TARGET} spdlog)

# The server runs on the target device to execute the tests
add_executable(${SERVER_TARGET}
  server/main.cpp
  server/tests.cpp)
set_target_properties(
  ${SERVER_TARGET}
  PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_GUI_IDENTIFIER "Chainblocks Remote Server"
  MACOSX_BUNDLE_BUNDLE_NAME "xyz.fragcolor.remote"
  MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
)

target_link_libraries(${SERVER_TARGET} ${SHARED_TARGET} SDL2-static SDL2main spdlog Catch2)