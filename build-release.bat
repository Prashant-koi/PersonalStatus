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
copy build\PersonalStatusMonitor.exe dist\ >nul
echo.
echo ✓ Distribution build complete!
echo ✓ Executable: dist\PersonalStatusMonitor.exe

REM --- BEGIN CODE SIGNING (SELF-SIGNED) ---
echo.
echo Attempting to self-sign the executable...

SETLOCAL EnableDelayedExpansion

REM --- CONFIGURATION FOR SIGNING ---
set CERT_PFX_PATH_RAW=C:\Users\koira\Programming\Portfolio\PersonalStatusMonitorDev.pfx
set CERT_PFX_PASS=PasswordHere
set SIGNTOOL_PATH_RAW=C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x86\signtool.exe
set TARGET_EXE_PATH_RAW=dist\PersonalStatusMonitor.exe
REM --- END CONFIGURATION ---

echo Debug: CERT_PFX_PATH_RAW is %CERT_PFX_PATH_RAW%
echo Debug: SIGNTOOL_PATH_RAW is %SIGNTOOL_PATH_RAW%
echo Debug: TARGET_EXE_PATH_RAW is %TARGET_EXE_PATH_RAW%

REM Check 1: PFX file
if not exist "%CERT_PFX_PATH_RAW%" (
    echo ✗ Certificate PFX file not found at "%CERT_PFX_PATH_RAW%".
    echo   Please create it using the PowerShell script first. Skipping signing.
    goto EndSigning
)

REM Check 2: Signtool
if not exist "%SIGNTOOL_PATH_RAW%" (
    echo ✗ signtool.exe not found at "%SIGNTOOL_PATH_RAW%".
    echo   Please verify the Windows SDK installation and path. Skipping signing.
    goto EndSigning
)

REM Check 3: Target Exe
if not exist "%TARGET_EXE_PATH_RAW%" (
    echo ✗ Executable "%TARGET_EXE_PATH_RAW%" not found. Skipping signing.
    goto EndSigning
)

REM If all checks pass, proceed to sign
echo All checks passed. Proceeding to sign "%TARGET_EXE_PATH_RAW%"...

REM Quoted versions for command execution (defined just before use)
set CERT_PFX_PATH_QUOTED="%CERT_PFX_PATH_RAW%"
set SIGNTOOL_PATH_QUOTED="%SIGNTOOL_PATH_RAW%"
set TARGET_EXE_PATH_QUOTED="%TARGET_EXE_PATH_RAW%"

%SIGNTOOL_PATH_QUOTED% sign /f %CERT_PFX_PATH_QUOTED% /p "%CERT_PFX_PASS%" /tr http://timestamp.sectigo.com /td sha256 /fd sha256 %TARGET_EXE_PATH_QUOTED%

if %ERRORLEVEL% EQU 0 (
    echo ✓ Executable signed successfully with self-signed certificate.
) else (
    echo ✗ Failed to sign executable. Errorlevel: %ERRORLEVEL%
    echo   Make sure signtool.exe is correctly installed (Windows SDK).
    echo   Verify the PFX path (%CERT_PFX_PATH_QUOTED%) and its password.
    echo   Command attempted: %SIGNTOOL_PATH_QUOTED% sign /f %CERT_PFX_PATH_QUOTED% /p "***PASSWORD_HIDDEN***" /tr http://timestamp.sectigo.com /td sha256 /fd sha256 %TARGET_EXE_PATH_QUOTED%
)

:EndSigning
ENDLOCAL
REM --- END CODE SIGNING ---

echo.
echo ✓ Size: 
dir dist\PersonalStatusMonitor.exe | find ".exe"

echo.
echo SUCCESS: Executable ready for distribution!