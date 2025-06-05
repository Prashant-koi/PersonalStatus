@echo off
echo Building Personal Status Monitor (Cross-Platform)...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
cmake -G "MinGW Makefiles" ..
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
cmake --build .
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ Build successful! Run personal_status_monitor.exe
) else (
    echo.
    echo ✗ Build failed!
)

pause