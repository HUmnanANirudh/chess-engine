cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(chess_engine main.cpp chess.cpp)

# Include directories
target_include_directories(chess_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler flags
if(MSVC)
    target_compile_options(chess_engine PRIVATE /W4)
else()
    target_compile_options(chess_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
