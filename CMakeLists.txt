cmake_minimum_required(VERSION 3.16)
project(TablutBot)

add_executable(TablutBot
    src/connection/Action.cpp
    src/connection/Socket.cpp
    src/state/Tablut.cpp
    
    src/main.cpp    
)

target_include_directories(TablutBot PRIVATE src/ lib/json/include)

add_subdirectory(lib/json)
