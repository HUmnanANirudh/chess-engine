@echo off
REM Chess Engine Build Script for Windows

echo Building Chess Engine...
g++ -std=c++17 -Wall -Wextra -O2 -o chess_engine.exe main.cpp chess.cpp

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo.
    echo To run the game, execute: chess_engine.exe
) else (
    echo.
    echo Build failed!
    exit /b 1
)
