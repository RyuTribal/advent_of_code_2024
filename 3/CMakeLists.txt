cmake_minimum_required(VERSION 3.16)
project(AOC)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(src)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

file(GLOB_RECURSE SRC_FILES src/*.c)

message(STATUS "Source files: ${SRC_FILES}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(RELEASE)
endif()

if(WIN32)
    add_compile_definitions("PLATFORM_WINDOWS=${WIN32}")
else()
    add_compile_definitions("PLATFORM_LINUX=${UNIX}")
endif()

add_executable(AOC ${SRC_FILES})