# Personal Status Monitor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-blue.svg)](https://github.com/Prashant-koi/personal-status-monitor)
[![Language](https://img.shields.io/badge/language-C++17-blue.svg)](https://isocpp.org/)
[![Latest Release](https://img.shields.io/github/v/release/Prashant-koi/PersonalStatus)](https://github.com/Prashant-koi/PersonalStatus/releases/latest)

A real-time desktop widget that shows your current thoughts and active applications on your portfolio website.

Would appreciate a GitHub star if you are reading this :)

## 🌟 Features (v0.1.0 - Windows Release)

This app creates a desktop widget that runs in the background with system tray integration:

-   📝 **Real-time Thoughts Sharing**: Type what you're thinking, and visitors see it instantly on your portfolio.
-   🟢 **Availability Status**: Easily toggle between "Free" and "Busy" states.
-   🖥️ **Application Monitoring**: Automatically detects and displays key applications you're currently using (e.g., VS Code, browsers).
-   🚀 **Live Portfolio Integration**: Updates your website (via a Vercel API endpoint or similar) every few seconds.
-   🔒 **Secure API Communication**: Uses API key authentication for sending data to your portfolio.
-   💾 **Persistent Settings**: Remembers your API endpoint and key (securely stored in Windows Registry).
-   ✨ **User-Friendly Setup**:
    *   **First-Run Setup Dialog**: Guides users through initial configuration.
    *   **Settings Dialog**: Allows users to update API endpoint and key later via the overlay window.
-   🎯 **System Tray Integration (Windows)**:
    *   Runs unobtrusively in the system tray (notification area).
    *   Left-click tray icon to show/hide the main status window.
    *   Right-click tray icon for a context menu (Show/Hide/Exit).
-   🎨 **Customizable Overlay Window**:
    *   Semi-transparent, draggable window to display and input status.
    *   Includes "Settings" button to reconfigure API details.
-   🧹 **Graceful Shutdown**: Ensures all threads and resources are cleaned up properly on exit.
-   📦 **Standalone Executable (Windows)**: Distributed as a single `.exe` file, statically linked with no external dependencies required.
-   🌍 **Cross-Platform Foundation**: Built with C++17 and CMake, with Linux support planned.

## 🚀 Getting Started (Using the Release Package - Recommended for Users)

This is the easiest way to use Personal Status Monitor on Windows. You'll download a pre-built package containing the application.

1.  **Download the Latest Release:**
    *   Go to the [Releases Page](https://github.com/yourusername/PersonalStatus/releases/latest).
    *   Download the `PersonalStatusMonitor.zip` file.

2.  **Extract the Application:**
    *   Extract the `PersonalStatusMonitor.zip` file to any folder on your computer.
    *   This will create a `PersonalStatusMonitor` folder. Inside this folder, you'll find `PersonalStatusMonitor.exe` along with other files like `README.md`, `LICENSE.txt`, and `QUICK_START.md`.

3.  **Run the Application:**
    *   Open the extracted `PersonalStatusMonitor` folder.
    *   Double-click `PersonalStatusMonitor.exe` to start the application.

4.  **First-Time Setup:**
    *   On the first run, a **Setup Dialog** will appear.
    *   **Enter your Portfolio API Endpoint**: This is the URL your portfolio website uses to receive status updates (e.g., `https://your-portfolio.vercel.app/api/status`).
    *   **Enter your API Key**: This is the secret key used to authenticate with your API endpoint. This key should match the one configured on your portfolio's backend.
    *   Click "Save & Start". The application will save these settings (in the Windows Registry) and start.

5.  **Usage:**
    *   The application will run in the system tray (notification area).
    *   Left-click the tray icon to toggle the visibility of the status overlay window.
    *   Right-click the tray icon for options like "Show Window", "Hide Window", and "Exit".
    *   Use the overlay window to type your thoughts, toggle your busy status, or access the "Settings" button to change your API configuration if needed.
    *   *Note: When running the application this way, console output is generally not visible as it's a GUI application.*

## 🛠️ Getting Started (Building from Source / CLI - For Developers)

These instructions are for developers who wish to build the application from its source code or run it in a way that shows console output for debugging.

### **Prerequisites**
-   **Windows:** MinGW/GCC (ucrt64 recommended, e.g., from MSYS2), CMake 3.16+
-   **Linux (Planned):** GCC, CMake 3.16+, X11 development libraries, libappindicator, libnotify
-   **Portfolio Setup:** A Vercel (or similar) hosted portfolio website with an API endpoint (`/api/status`) capable of receiving POST requests with JSON data (authenticated with an API key) and handling GET requests to display the status.

### **1. Clone the Repository**
```bash
git clone https://github.com/yourusername/PersonalStatus.git
cd PersonalStatus
```

### **2. Build the Application**

**Windows (Development Build):**
```bash
# This script cleans, configures with CMake, and builds a development version.
.\build.bat

# The script creates a 'build' directory.
# The development executable will be: build\PersonalStatusMonitor.exe
```
*For creating a distributable release package (including static linking and packaging), use `package-release.bat` after ensuring `build-release.bat` is configured for signing if desired.*

**Linux (Planned):**
```bash
# (Instructions for Linux will be added when support is complete)
# chmod +x build.sh
# ./build.sh
# ./build/personal_status_monitor
```

### **3. Run the Application (from CLI)**

**Windows:**
```bash
# Run the development executable from the project root:
.\build\PersonalStatusMonitor.exe

# Running from the command line will show console output, useful for debugging.
# On the first run (or if settings are cleared from the Registry), 
# the Setup Dialog will appear to configure the API endpoint and key.
```

## 🧪 Testing Your Setup

### **1. Verify Build Success (for source builds)**
If building from source using `build.bat`, you should see output similar to:
```
✓ Build successful! Run personal_status_monitor.exe
```

### **2. Check Application Startup (when run from CLI)**
When running the development executable from a terminal (e.g., `.\build\PersonalStatusMonitor.exe`):
Expected console output might include:
```
Personal Status Monitor - Desktop Widget
Attempting to load settings from system storage...
Settings loaded from system storage
API URL: https://your-saved-api-url.com/api/status

Starting local web server...
Web server started on port 8081
Creating overlay window...
System tray icon created successfully!
Starting Vercel API push loop...
All components started. Running in background...
```
Or, on first run / if settings are not found:
```
Personal Status Monitor - Desktop Widget
Attempting to load settings from system storage...
No settings found or settings are invalid. Launching setup dialog...
(Setup Dialog Appears, application continues after setup)
```
*(Actual messages may vary slightly based on application flow and settings state. If running the release `.exe` by double-clicking, this console output will not be visible.)*

### **3. Test Components**

**Tray Icon:**
- [✅] Icon appears in system tray (notification area)
- [✅] Left-click toggles window visibility
- [✅] Right-click shows context menu
- [✅] Tooltip shows "Personal Status Monitor"

**Desktop Widget:**
- [✅] Semi-transparent window appears (bottom-right corner)
- [✅] Text input field works
- [✅] "Free/Busy" button toggles correctly
- [✅] Window is draggable

**Local Server Test:**
```bash
# Should return JSON status
curl http://localhost:8081

# Or visit in browser
http://localhost:8081
```

### **4. Troubleshooting**

**Build Issues:**
```bash
# Windows - Missing libraries
# Install MinGW with: winlibs.com
# Ensure PATH includes: C:\mingw64\bin

# Linux - Missing dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libx11-dev
```

**Runtime Issues:**
```bash
# API connection fails
# Check Vercel deployment status
# Verify API key matches Vercel environment variable

# GUI doesn't appear
# Try running as administrator (Windows)
# Check X11 forwarding (Linux remote)
```

## 📸 Example Output

### Console Output
```
[VERCEL] ✓ Sent: {"timestamp":1701234567,"thoughts":"Working on a React component","activeApps":["Visual Studio Code","Brave Browser"],"busy":false}
[VERCEL] ✓ Sent: {"timestamp":1701234569,"thoughts":"Debugging API integration","activeApps":["Visual Studio Code","Postman"],"busy":true}
```

### Portfolio Website Display
```
🟢 Online

Current Thoughts: "Working on a React component"
Availability: 🟢 Available  
Currently Using: Visual Studio Code, Brave Browser
Last updated: 2:34:27 PM
```

### API Response
```json
{
  "thoughts": "Working on a React component",
  "activeApps": ["Visual Studio Code", "Brave Browser"],
  "busy": false,
  "timestamp": 1701234567,
  "status": "online"
}
```

## 🏗️ Architecture

### **System Overview**
```
┌─────────────────────────────────────────────────────────────────┐
│                        Personal Status Monitor                  │
│                     (Cross-Platform Application)                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐    ┌──────────────────┐    ┌─────────────┐ │
│  │   Desktop GUI   │    │   Local Server   │    │ Config Mgr  │ │
│  │ (Platform-Spec) │◄──►│   Port 8081      │◄──►│ (.env file) │ │
│  │                 │    │   JSON API       │    │ Environment │ │
│  │ Windows: Win32  │    │   CORS Enabled   │    │ Variables   │ │
│  │ Linux: X11      │    │   Thread Safe    │    │             │ │
│  └─────────────────┘    └──────────────────┘    └─────────────┘ │
│           │                       │                       │     │
│           ▼                       ▼                       ▼     │
│  ┌─────────────────┐    ┌──────────────────┐    ┌─────────────┐ │
│  │ Thoughts Manager│    │  App Detector    │    │ HTTP Client │ │
│  │ (Thread Safe)   │    │ (Process Scan)   │    │ (Platform)  │ │
│  │ • Text Storage  │    │ • Windows: TH32  │    │ Win: WinHTTP│ │
│  │ • Status Toggle │    │ • Linux: /proc   │    │ Linux: curl │ │
│  │ • Mutex Locked  │    │ • Real-time Poll │    │ • API Auth  │ │
│  └─────────────────┘    └──────────────────┘    └─────────────┘ │
│                                   │                       │     │
│                                   └───────────────────────┼─────┤
│                                                           │     │
└───────────────────────────────────────────────────────────┼─────┘
                                                            │
                              Every 2 seconds              │
                                    │                       │
                                    ▼                       ▼
              ┌─────────────────────────────────────────────────────┐
              │              Vercel API Endpoint                    │
              │        https://your-portfolio.vercel.app            │
              │                                                     │
              │  POST /api/status (🔒 API Key Required)            │
              │  • Updates current status                           │
              │  • Validates authentication                         │
              │  • Stores in memory                                 │
              │                                                     │
              │  GET /api/status (🌍 Public Access)               │
              │  • Returns current status                           │
              │  • Portfolio visitors can view                      │
              │  • No authentication needed                         │
              └─────────────────────────────────────────────────────┘
                                    │
                                    ▼
              ┌─────────────────────────────────────────────────────┐
              │            Your Portfolio Website                   │
              │                                                     │
              │  🟢 Online                                         │
              │  Current Thoughts: "Building cross-platform app"   │
              │  Availability: 🟢 Available                       │
              │  Currently Using: VS Code, Brave Browser           │
              │  Last updated: 2:34:27 PM                          │
              └─────────────────────────────────────────────────────┘
```

### **Component Details**

#### **Cross-Platform Abstraction Layer**
```cpp
// Abstract base class
class OverlayWindow {
public:
    virtual bool create() = 0;
    virtual void show() = 0;
    virtual void messageLoop() = 0;
    // Factory method creates platform-specific instance
    static OverlayWindow* createPlatformWindow();
};

#ifdef _WIN32
    return new OverlayWindow_Win32();  // Windows implementation
#elif __linux__
    return new OverlayWindow_X11();    // Linux implementation
#endif
```

#### **Platform-Specific Implementations**

| Component | Windows | Linux | Shared |
|-----------|---------|-------|---------|
| **GUI Framework** | Win32 API | X11/Xlib | Abstract Interface |
| **HTTP Client** | WinHTTP | libcurl/sockets | JSON Generation |
| **Process Detection** | CreateToolhelp32Snapshot | /proc filesystem | App Matching Logic |
| **Threading** | std::thread | std::thread | Thread Management |
| **File I/O** | Windows API | POSIX | Config Parser |
| **Build System** | CMake + MinGW | CMake + GCC | CMakeLists.txt |

#### **Data Flow**
1. **User Input** → Desktop GUI captures thoughts/status
2. **App Detection** → Scans running processes every 2 seconds  
3. **JSON Generation** → Combines user data + detected apps
4. **Local Server** → Serves data on localhost:8081 for development
5. **API Push** → Sends encrypted data to Vercel endpoint
6. **Portfolio Display** → Website fetches and displays live status

#### **Security Model**
```
┌─────────────────┐    🔒 API Key    ┌─────────────────┐
│   Your C++ App  │ ────────────────► │  Vercel API     │
│   (Private)     │    POST /status   │  (Authenticated)│
└─────────────────┘                   └─────────────────┘
                                               │
                  ┌─────────────────────────────┘
                  │
                  ▼ GET /status (No Auth)
        ┌─────────────────┐
        │ Portfolio       │
        │ Visitors        │
        │ (Public)        │
        └─────────────────┘
```

### **File Structure**
```
PersonalStatus/
├── src/
│   ├── common/                    # Cross-platform code
│   │   ├── AppDetector.h/cpp     # Process detection logic
│   │   ├── ThoughtsManager.h/cpp # Thread-safe data management
│   │   ├── WebServer.h/cpp       # Local HTTP server
│   │   ├── OverlayWindow.h/cpp   # Abstract GUI interface
│   │   └── config.h              # Environment variable parser
│   ├── windows/                   # Windows-specific implementation
│   │   └── OverlayWindow_Win32.h/cpp
│   ├── linux/                     # Linux-specific implementation  
│   │   └── OverlayWindow_X11.h/cpp
│   └── main.cpp                   # Entry point & main loop
├── build/                         # Generated build files (gitignored)
├── .env                          # Your configuration (gitignored)
├── .env.example                  # Template for others
├── CMakeLists.txt                # Cross-platform build config
├── build.bat                     # Windows build script
├── build.sh                      # Linux build script
└── README.md                     # This file
```

### **Modern C++ Features Used**
- ✅ **C++17 Standard** - Modern language features
- ✅ **RAII** - Automatic resource management
- ✅ **Smart Pointers** - Memory safety
- ✅ **Abstract Base Classes** - Clean interfaces
- ✅ **Factory Pattern** - Platform-specific object creation
- ✅ **std::thread** - Cross-platform threading
- ✅ **Conditional Compilation** - `#ifdef` for platform code
- ✅ **CMake** - Modern build system
