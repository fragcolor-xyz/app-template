# set(fragments_SOURCES main.cpp)
set(fragments_SOURCES main1.cpp)

if(ANDROID)
    add_library(fragments SHARED ${fragments_SOURCES})
else()
    add_executable(fragments ${fragments_SOURCES})
endif()

target_link_libraries(fragments spdlog Boost::algorithm gfx gfx_imgui cbl-static)
target_bundle_files(fragments PRIVATE bundled entry.edn)
