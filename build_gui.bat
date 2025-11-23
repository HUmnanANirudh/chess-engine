@echo off
REM Chess Engine GUI Build Script for Windows with Qt5

echo Building Chess Engine GUI...
echo.

REM Check if Qt5 is installed
where qmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Qt5 not found in PATH
    echo Please install Qt5 and add it to your PATH
    echo Or set QT5_DIR environment variable
    exit /b 1
)

REM Create build directory
if not exist build_gui (
    mkdir build_gui
)

cd build_gui

REM Configure with CMake
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%QT5_DIR%

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b 1
)

REM Build
cmake --build . --config Release

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo.
    echo To run the GUI, execute: chess_engine_gui.exe
) else (
    echo.
    echo Build failed!
    exit /b 1
)

cd ..
