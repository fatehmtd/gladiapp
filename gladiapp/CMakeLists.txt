cmake_minimum_required(VERSION 3.16)

project(GLADIAPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GNUInstallDirs)

# Modern CMake: Use Boost's imported targets and avoid FindBoost
find_package(boost_system REQUIRED CONFIG)
find_package(boost_thread REQUIRED CONFIG)
find_package(boost_beast REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)

# Debug: Print Boost information
message(STATUS "Boost_FOUND: ${Boost_FOUND}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL or set OPENSSL_ROOT_DIR.")
endif()

# fetch SPDLOG for logging
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG "v1.15.0"
)
FetchContent_MakeAvailable(spdlog)

# fetch nlohmann for json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG "v3.12.0"
)
FetchContent_MakeAvailable(nlohmann_json)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

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
    Boost::system
    Boost::thread
    ${OPENSSL_LIBRARIES}
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(gladiapp
    BASE_NAME gladiapp
    EXPORT_FILE_NAME gladiapp_export.h
)

target_include_directories(gladiapp PUBLIC
    $<INSTALL_INTERFACE:include>  # For installed package (optional but good practice)
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

