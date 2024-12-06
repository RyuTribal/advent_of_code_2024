@echo off
set BUILD_DIR=build
set BUILD_TYPE=Debug

where cmake >nul 2>nul
if errorlevel 1 (
    echo CMake not found in PATH. Please install or add it to PATH.
    exit /b 1
)

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

cmake ..
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

cmake --build .
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

cd ..

echo Build complete!

%BUILD_DIR%\Debug\AOC.exe