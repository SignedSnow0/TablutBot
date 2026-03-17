cmake_minimum_required(VERSION 3.16)
project(TablutBot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_executable(TablutBot
    src/connection/Socket.cpp
    src/connection/TablutReader.cpp
    src/connection/TablutWriter.cpp

    src/state/Tablut.cpp
    
    src/utils/Logger.cpp

    src/main.cpp    
)

option(NO_LOG "Disables logging" OFF)

if(NO_LOG)
    add_definitions(-DNO_LOG)
    message(STATUS "Excluding logs")
endif()

target_include_directories(TablutBot PRIVATE src/ lib/json/include)

add_subdirectory(lib/json)
