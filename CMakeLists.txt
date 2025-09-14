cmake_minimum_required(VERSION 3.16)

project(GRADIAPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent for external dependencies
include(FetchContent)

# Fetch dotenv-cpp (header-only library)
FetchContent_Declare(
    dotenv-cpp
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(dotenv-cpp)

# Create an interface library for dotenv-cpp since it's header-only
add_library(dotenv-cpp INTERFACE)
target_include_directories(dotenv-cpp INTERFACE 
    ${dotenv-cpp_SOURCE_DIR}/include
)

# Set default installation prefix if not specified
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
endif()

# the main library
add_subdirectory(gladiapp)

# the examples
add_subdirectory(examples/example-rest)
add_subdirectory(examples/example-ws)