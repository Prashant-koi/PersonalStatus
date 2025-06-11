# Personal Status Monitor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-blue.svg)](https://github.com/yourusername/personal-status-monitor)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)

A real-time desktop widget that shows your current thoughts and active applications on your portfolio website.
Would appreciate a GitHub star if you are reading this :)

## 🎯 What It Does

This app creates a cross-platform desktop widget that runs in the background with system tray integration:

- 📝 **Real-time thoughts sharing** - Type what you're thinking, visitors see it instantly
- 🟢 **Availability status** - Toggle between busy/free
- 🖥️ **App monitoring** - Automatically detects development tools you're using
- 🚀 **Live portfolio integration** - Updates your website every 2 seconds
- 🔒 **Secure API** - Protected with API key authentication
- 🌍 **Cross-platform** - Works on Windows and Linux
- 🎯 **System Tray Integration** - Runs in background, accessible via tray icon
- 🔄 **Show/Hide Toggle** - Left-click tray icon to show/hide window
- 📱 **Context Menu** - Right-click tray for options (Show/Hide/Exit)
- 🧹 **Graceful Shutdown** - Clean exit with proper thread cleanup

## 🚀 Quick Start

### **Prerequisites**
- **Windows:** MinGW/GCC, CMake 3.16+
- **Linux:** GCC, CMake 3.16+, X11 development libraries
- **Both:** Vercel account, Next.js portfolio website

### **1. Clone and Setup**
```bash
git clone https://github.com/yourusername/personal-status-monitor
cd personal-status-monitor

# Copy environment template
cp .env.example .env

# Edit with your values
notepad .env  # Windows
nano .env     # Linux
```

### **2. Configure Your API**
```bash
# In .env file:
VERCEL_API_URL=https://your-project.vercel.app/api/status
API_KEY=your-secret-api-key
```

### **3. Build and Run**

**Windows:**
```bash
# Using build script (recommended)
.\build.bat

# The script creates a 'build' directory automatically
# Executable will be: build\personal_status_monitor.exe

# Run from project root (so it finds .env file)
.\build\personal_status_monitor.exe
# Application starts with:
# System tray icon (notification area)
# Desktop overlay window (can be hidden)
```

**Linux:**
```bash
# Using build script (installs dependencies)
chmod +x build.sh
./build.sh

# Or manually with CMake
mkdir build && cd build
cmake ..
make
./personal_status_monitor
```

## 🧪 Testing Your Setup

### **1. Verify Build Success**
You should see:
```
✓ Build successful! Run personal_status_monitor.exe
```

### **2. Check Application Startup**
Expected console output:
```
Personal Status Monitor - Desktop Widget
========================================
Configuration loaded from .env file
API URL: https://your-project.vercel.app/api/status

Starting local web server...
Web server started on port 8081
Creating overlay window...
All components started. GUI running...
System tray icon created successfully!
If icon is hidden, drag it from overflow area to make it always visible        
Creating overlay window...
[WINDOW] Shown and brought to top
Starting Vercel API push loop...
All components started. Running in background...
Right-click tray icon for options.

[VERCEL] ✓ Sent: {"timestamp":1701234567,"thoughts":"","activeApps":["Visual Studio Code"],"busy":false}
```

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
# Missing .env file
cp .env.example .env
# Edit with your actual values

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
