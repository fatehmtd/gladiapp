cmake_minimum_required(VERSION 3.16)

project(GLADIAPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GNUInstallDirs)

# Modern CMake: Use Boost's imported targets and avoid FindBoost
find_package(Boost REQUIRED COMPONENTS system thread CONFIG)
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

add_executable(GLADIAPP
    main.cpp
    gladiapp_rest.cpp
)

target_link_libraries(GLADIAPP
    PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::thread
    ${OPENSSL_LIBRARIES}
)
