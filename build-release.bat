@echo off
echo Building Personal Status Monitor for Distribution...
echo ================================================

REM Clean previous builds
if exist dist rmdir /s /q dist
if exist build rmdir /s /q build

REM Create directories
mkdir build
mkdir dist

cd build

REM Configure for release with static linking
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

cd ..

REM Copy executable to dist folder
copy build\PersonalStatusMonitor.exe dist\
echo.
echo ✓ Distribution build complete!
echo ✓ Executable: dist\PersonalStatusMonitor.exe
echo ✓ Size: 
dir dist\PersonalStatusMonitor.exe | find ".exe"

echo.
echo SUCCESS: Executable ready for distribution!