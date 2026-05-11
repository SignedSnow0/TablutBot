cmake_minimum_required(VERSION 3.16)
project(TablutBot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

option(USE_LOGS "Enables the logger system" ON)

add_executable(TablutBot
    src/ai/Evaluations.cpp
    src/ai/Heuristic.cpp
    src/ai/Minimax.cpp
    src/ai/ThreadPool.hpp
    src/ai/TranspositionTable.cpp

    src/connection/Socket.cpp
    src/connection/TablutReader.cpp
    src/connection/TablutWriter.cpp

    src/state/Tablut.cpp
    src/state/Utils.cpp
    
    src/utils/Logger.cpp

    src/main.cpp    
)


if(USE_LOGS)
    message(STATUS "Compiling with logs")
else()
    add_definitions(-DNO_LOG)
    message(STATUS "Excluding logs")
endif()

target_include_directories(TablutBot PRIVATE src/ lib/json/include)

add_subdirectory(lib/json)
