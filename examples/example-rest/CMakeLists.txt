cmake_minimum_required(VERSION 3.16)

project(gladiapp_rest_example VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_executable(gladiapp_rest_example main.cpp)

target_link_libraries(gladiapp_rest_example PRIVATE
    gladiapp
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Copy the testing.wav file to the output directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testing.wav 
               ${CMAKE_CURRENT_BINARY_DIR}/testing.wav 
               COPYONLY)

# For multi-config generators (like Xcode/Visual Studio), also copy to config-specific directories
if(CMAKE_CONFIGURATION_TYPES)
    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testing.wav 
                       ${CMAKE_CURRENT_BINARY_DIR}/${config}/testing.wav 
                       COPYONLY)
    endforeach()
endif()

# Installation rules
install(TARGETS gladiapp_rest_example
        RUNTIME DESTINATION bin)

install(FILES testing.wav
        DESTINATION bin)
