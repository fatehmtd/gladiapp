cmake_minimum_required(VERSION 3.16)

project(GLADIAPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GNUInstallDirs)

# A parent build (e.g. VASDK_ANDROID, which builds curl from source and
# aliases it to CURL::libcurl since Android has no system curl for
# find_package's module-mode FindCURL.cmake to discover) may already provide
# this target — reuse it instead of forcing another lookup.
if(NOT TARGET CURL::libcurl)
    find_package(CURL REQUIRED)

    if(NOT CURL_FOUND)
        message(FATAL_ERROR "curl not found. Please install curl (built with SSL and WebSocket support).")
    endif()

    message(STATUS "CURL_VERSION_STRING: ${CURL_VERSION_STRING}")
else()
    message(STATUS "Using existing CURL::libcurl target")
endif()

# Fetch spdlog and nlohmann-json from source, reusing a parent build's
# targets the same way CURL::libcurl is reused above.
if(NOT TARGET spdlog::spdlog)
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Disable spdlog examples" FORCE)
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Disable spdlog tests" FORCE)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.1
    )
    FetchContent_MakeAvailable(spdlog)
else()
    message(STATUS "Using existing spdlog::spdlog target")
endif()

if(NOT TARGET nlohmann_json::nlohmann_json)
    set(JSON_BuildTests OFF CACHE INTERNAL "Disable nlohmann-json tests" FORCE)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
    )
    FetchContent_MakeAvailable(nlohmann_json)
else()
    message(STATUS "Using existing nlohmann_json::nlohmann_json target")
endif()

# fetch base64
set(BASE64_ENABLE_TESTING OFF CACHE BOOL "Disable base64 testing" FORCE)
FetchContent_Declare(
    base64
    GIT_REPOSITORY https://github.com/tobiaslocker/base64.git
)
FetchContent_MakeAvailable(base64)

add_library(gladiapp STATIC
    # error
    src/gladiapp_error.cpp
    # rest
    src/gladiapp_rest.cpp
    src/gladiapp_rest_request.cpp
    src/gladiapp_rest_response.cpp
    # websockets
    src/gladiapp_ws.cpp
    src/gladiapp_ws_request.cpp
    src/gladiapp_ws_response.cpp
)

target_include_directories(gladiapp PRIVATE include)

target_link_libraries(gladiapp
    PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    CURL::libcurl
    base64
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(gladiapp
    BASE_NAME gladiapp
    EXPORT_FILE_NAME gladiapp_export.h
)

target_include_directories(gladiapp PUBLIC
    $<INSTALL_INTERFACE:include> # For installed package (optional but good practice)
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # For direct build
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # To export the symbols header
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/gladiapp # to simplify the internal include files
)

# Installation rules
install(TARGETS gladiapp
    EXPORT gladiappTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "impl" EXCLUDE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gladiapp_export.h
    DESTINATION include/gladiapp)

