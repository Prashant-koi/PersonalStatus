@echo off
echo Creating Distribution Package...
echo ================================

REM Build first
call build-release.bat

REM Check if build was successful
if not exist dist\PersonalStatusMonitor.exe (
    echo Build failed - executable not found!
    pause
    exit /b 1
)

echo.
echo Packaging files...

REM Create package structure
if exist PersonalStatusMonitor rmdir /s /q PersonalStatusMonitor
mkdir PersonalStatusMonitor

REM Copy executable
echo Copying executable...
copy dist\PersonalStatusMonitor.exe PersonalStatusMonitor\ >nul

REM Copy or create README.md
if exist README.md (
    echo Copying README.md...
    copy README.md PersonalStatusMonitor\ >nul
) else (
    echo Creating basic README.md...
    echo # Personal Status Monitor > PersonalStatusMonitor\README.md
    echo. >> PersonalStatusMonitor\README.md
    echo Desktop widget for real-time portfolio integration. >> PersonalStatusMonitor\README.md
    echo. >> PersonalStatusMonitor\README.md
    echo ## Quick Start >> PersonalStatusMonitor\README.md
    echo 1. Double-click PersonalStatusMonitor.exe >> PersonalStatusMonitor\README.md
    echo 2. Enter your portfolio API endpoint in the setup dialog >> PersonalStatusMonitor\README.md
    echo 3. Enter your API key for authentication >> PersonalStatusMonitor\README.md
    echo 4. Click "Save & Start" >> PersonalStatusMonitor\README.md
    echo 5. App runs in system tray - right-click for options >> PersonalStatusMonitor\README.md
)

REM Create LICENSE.txt (always create since it might not exist)
echo Creating LICENSE.txt...
echo MIT License > PersonalStatusMonitor\LICENSE.txt
echo. >> PersonalStatusMonitor\LICENSE.txt
echo Copyright (c) 2024 Prasant Koirala >> PersonalStatusMonitor\LICENSE.txt
echo. >> PersonalStatusMonitor\LICENSE.txt
echo Permission is hereby granted, free of charge, to any person obtaining a copy >> PersonalStatusMonitor\LICENSE.txt
echo of this software and associated documentation files (the "Software"), to deal >> PersonalStatusMonitor\LICENSE.txt
echo in the Software without restriction, including without limitation the rights >> PersonalStatusMonitor\LICENSE.txt
echo to use, copy, modify, merge, publish, distribute, sublicense, and/or sell >> PersonalStatusMonitor\LICENSE.txt
echo copies of the Software, and to permit persons to whom the Software is >> PersonalStatusMonitor\LICENSE.txt
echo furnished to do so, subject to the following conditions: >> PersonalStatusMonitor\LICENSE.txt
echo. >> PersonalStatusMonitor\LICENSE.txt
echo The above copyright notice and this permission notice shall be included in all >> PersonalStatusMonitor\LICENSE.txt
echo copies or substantial portions of the Software. >> PersonalStatusMonitor\LICENSE.txt
echo. >> PersonalStatusMonitor\LICENSE.txt
echo THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR >> PersonalStatusMonitor\LICENSE.txt
echo IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, >> PersonalStatusMonitor\LICENSE.txt
echo FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE >> PersonalStatusMonitor\LICENSE.txt
echo AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER >> PersonalStatusMonitor\LICENSE.txt
echo LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, >> PersonalStatusMonitor\LICENSE.txt
echo OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE >> PersonalStatusMonitor\LICENSE.txt
echo SOFTWARE. >> PersonalStatusMonitor\LICENSE.txt

REM Create QUICK_START.md
echo Creating QUICK_START.md...
echo # Personal Status Monitor - Quick Start Guide > PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo ## What is this? >> PersonalStatusMonitor\QUICK_START.md
echo Personal Status Monitor is a desktop widget that shows your current >> PersonalStatusMonitor\QUICK_START.md
echo activity and thoughts on your portfolio website in real-time. >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo ## Quick Setup (5 minutes) >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo 1. **Double-click PersonalStatusMonitor.exe** >> PersonalStatusMonitor\QUICK_START.md
echo    - Setup dialog will appear on first run >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo 2. **Enter your Portfolio API Endpoint:** >> PersonalStatusMonitor\QUICK_START.md
echo    - Example: https://your-portfolio.vercel.app/api/status >> PersonalStatusMonitor\QUICK_START.md
echo    - This is where your website receives status updates >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo 3. **Enter your API Key:** >> PersonalStatusMonitor\QUICK_START.md
echo    - Example: psk_your_secure_random_key_here >> PersonalStatusMonitor\QUICK_START.md
echo    - Use the same key in your website's environment variables >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo 4. **Click "Save & Start"** >> PersonalStatusMonitor\QUICK_START.md
echo    - Settings are saved automatically >> PersonalStatusMonitor\QUICK_START.md
echo    - App starts running in background >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo 5. **Find the system tray icon** >> PersonalStatusMonitor\QUICK_START.md
echo    - Look for the app icon in your notification area >> PersonalStatusMonitor\QUICK_START.md
echo    - Left-click to show/hide the status window >> PersonalStatusMonitor\QUICK_START.md
echo    - Right-click for menu options >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo ## Features >> PersonalStatusMonitor\QUICK_START.md
echo - Real-time thoughts sharing >> PersonalStatusMonitor\QUICK_START.md
echo - Availability status (Free/Busy toggle) >> PersonalStatusMonitor\QUICK_START.md
echo - Automatic app detection (shows what you're working with) >> PersonalStatusMonitor\QUICK_START.md
echo - Secure API integration with your portfolio >> PersonalStatusMonitor\QUICK_START.md
echo - System tray integration >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo ## Support >> PersonalStatusMonitor\QUICK_START.md
echo For detailed documentation, see README.md >> PersonalStatusMonitor\QUICK_START.md
echo. >> PersonalStatusMonitor\QUICK_START.md
echo Developed by Prasant Koirala ^| MIT License >> PersonalStatusMonitor\QUICK_START.md

REM Create ZIP package using PowerShell (more reliable method)
echo Creating ZIP package...
if exist PersonalStatusMonitor.zip del PersonalStatusMonitor.zip

REM Use PowerShell with full path and error handling
powershell -Command "try { Compress-Archive -Path 'PersonalStatusMonitor' -DestinationPath 'PersonalStatusMonitor.zip' -Force; Write-Host 'ZIP created successfully' } catch { Write-Host 'ZIP creation failed:' $_.Exception.Message; exit 1 }"

if %ERRORLEVEL% NEQ 0 (
    echo PowerShell ZIP creation failed, trying alternative method...
    REM Fallback: Use 7-Zip if available
    if exist "C:\Program Files\7-Zip\7z.exe" (
        "C:\Program Files\7-Zip\7z.exe" a PersonalStatusMonitor.zip PersonalStatusMonitor\*
        echo ZIP created with 7-Zip
    ) else (
        echo Warning: Could not create ZIP file. PowerShell and 7-Zip not available.
        echo You can manually compress the PersonalStatusMonitor folder.
    )
)

echo.
echo ================================
echo ✓ Distribution package complete!
echo ================================
echo.
echo Package contents:
dir PersonalStatusMonitor /b

if exist PersonalStatusMonitor.zip (
    echo.
    echo ZIP file created:
    dir PersonalStatusMonitor.zip
    echo.
    echo ✓ Ready for distribution: PersonalStatusMonitor.zip
) else (
    echo.
    echo ⚠ ZIP file not created - you can manually compress the folder
    echo ✓ Folder ready for distribution: PersonalStatusMonitor\
)

echo.
echo SUCCESS: Your app is ready to distribute!
pause