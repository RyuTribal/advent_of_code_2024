cmake_minimum_required(VERSION 3.16)
project(AOC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

file(GLOB SRC_FILES src/*.cpp)

add_executable(AOC ${SRC_FILES})

target_include_directories(AOC PRIVATE src)

if(WIN32)
    target_compile_definitions(AOC PRIVATE PLATFORM_WINDOWS=1)
elseif(UNIX)
    target_compile_definitions(AOC PRIVATE PLATFORM_LINUX=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(AOC PRIVATE DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(AOC PRIVATE RELEASE=1)
endif()
