# Personal Status Monitor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://github.com/Prashant-koi/PersonalStatus)
[![Language](https://img.shields.io/badge/language-C++17-blue.svg)](https://isocpp.org/)
[![Latest Release](https://img.shields.io/badge/version-0.1.0--windows-brightgreen)](https://github.com/Prashant-koi/PersonalStatus/releases/latest)

A real-time desktop widget that shows your current thoughts and active applications on your portfolio website.

> **Note:** This is the **Windows-only release (v0.1.0)**. Linux/Hyprland support coming in v0.2.0!

Would appreciate a GitHub star if you are reading this :)

## Features (v0.1.0 - Windows Release)

This app creates a desktop widget that runs in the background with system tray integration:

-   **Real-time Thoughts Sharing**: Type what you're thinking, and visitors see it instantly on your portfolio.
-   **Availability Status**: Easily toggle between "Free" and "Busy" states.
-   **Application Monitoring**: Automatically detects and displays key applications you're currently using (e.g., VS Code, browsers).
-   **Live Portfolio Integration**: Updates your website (via a Vercel API endpoint or similar) every few seconds.
-   **Secure API Communication**: Uses API key authentication for sending data to your portfolio.
-   **Persistent Settings**: Remembers your API endpoint and key (securely stored in Windows Registry).
-    **User-Friendly Setup**:
    *   **First-Run Setup Dialog**: Guides users through initial configuration.
    *   **Settings Dialog**: Allows users to update API endpoint and key later via the overlay window.
-   **System Tray Integration (Windows)**:
    *   Runs unobtrusively in the system tray (notification area).
    *   Left-click tray icon to show/hide the main status window.
    *   Right-click tray icon for a context menu (Show/Hide/Exit).
-   **Customizable Overlay Window**:
    *   Semi-transparent, draggable window to display and input status.
    *   Includes "Settings" button to reconfigure API details.
-   **Graceful Shutdown**: Ensures all threads and resources are cleaned up properly on exit.
-   **Standalone Executable (Windows)**: Distributed as a single `.exe` file, statically linked with no external dependencies required.
-   **Cross-Platform Foundation**: Built with C++17 and CMake, with Linux support planned.

## Getting Started (Windows Users)

### **Quick Start (Recommended)**
1. Download the latest `PersonalStatusMonitor.zip` from [Releases](https://github.com/Prashant-koi/PersonalStatus/releases)
2. Extract and run `PersonalStatusMonitor.exe`
3. Follow the setup wizard

### **Building from Source**

**Prerequisites:**
- Windows 10/11
- MinGW/GCC (ucrt64 recommended from MSYS2)
- CMake 3.16+

**Build Steps:**
```bash
git clone https://github.com/yourusername/PersonalStatus.git
cd PersonalStatus
.\build.bat
```

## 📋 Roadmap

- [x] Windows implementation (v0.1.0) ✅
- [ ] Hyprland/Wayland support (v0.2.0) - Target: November 2024
- [ ] GNOME/KDE support (v0.3.0)
- [ ] macOS support (v0.4.0)

## 💬 Linux Users

Linux support is actively being developed! Watch this repo to get notified when v0.2.0 (Hyprland/Wayland) is released.

**Want to contribute?** Check out the [`hyprland-wayland`](https://github.com/yourusername/PersonalStatus/tree/hyprland-wayland) branch.

## 🛠️ Getting Started (Using the Release Package - Recommended for Users)

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
    *   **PLEASE CONTINUE READING FOR INSTRUCTIONS TO SETUP YOUR PORTFOLIO WEBSITE BACKEND**

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

## 🌐 Setting Up Your Portfolio Backend (Required for Portfolio Integration)

To enable the Personal Status Monitor desktop application to send updates to your portfolio website, you need to set up an API endpoint on your site. This endpoint will receive data from the desktop app and make it available for your portfolio frontend to display.

**Both users of the pre-built application and developers building from source need to complete one of the following backend setups if they want to display their status on their portfolio.**

Choose the option that best fits your portfolio's technology stack:

---

### **Option 1: Next.js (App Router)**

This is recommended if your portfolio is already built with Next.js or if you plan to use it.

#### **1. Create the API Route File**

Create the following file in your Next.js project:

**Filepath:** `app/api/status/route.ts` (or `src/app/api/status/route.ts` if your `app` directory is inside `src`)

```typescript
// filepath: app/api/status/route.ts
import { NextRequest, NextResponse } from 'next/server';

// Your personal API key from environment variable
const API_KEY = process.env.PERSONAL_STATUS_API_KEY;

let currentStatus = {
  thoughts: "App offline",
  activeApps: [] as string[], // Ensure type for empty array
  busy: false,
  timestamp: 0,
  lastUpdated: null as Date | null
};

function validateApiKey(request: NextRequest): boolean {
  const apiKeyFromHeader = request.headers.get('X-API-Key');
  return apiKeyFromHeader === API_KEY;
}

export async function POST(request: NextRequest) {
  try {
    if (!API_KEY) {
      console.error('API Key not configured on the server.');
      return NextResponse.json(
        { error: 'Server configuration error: API Key missing' },
        { status: 500 }
      );
    }
    if (!validateApiKey(request)) {
      return NextResponse.json(
        { error: 'Unauthorized - Invalid API key' },
        { status: 401 }
      );
    }

    const data = await request.json();

    if (typeof data.thoughts === 'string' &&
        Array.isArray(data.activeApps) &&
        typeof data.busy === 'boolean' &&
        typeof data.timestamp === 'number') {

      currentStatus = {
        ...data,
        lastUpdated: new Date()
      };
      console.log('Status updated:', currentStatus);
      return NextResponse.json({
        success: true,
        message: 'Status updated successfully'
      });
    } else {
      return NextResponse.json(
        { error: 'Invalid data format' },
        { status: 400 }
      );
    }
  } catch (error) {
    console.error('Error updating status:', error);
    return NextResponse.json(
      { error: 'Failed to update status' },
      { status: 500 }
    );
  }
}

export async function GET() {
  try {
    const now = Date.now() / 1000;
    const isRecent = currentStatus.timestamp && (now - currentStatus.timestamp) < 15;

    if (!isRecent || currentStatus.thoughts === "App offline") {
      return NextResponse.json({
        thoughts: "App offline",
        activeApps: [],
        busy: false,
        timestamp: 0,
        lastUpdated: null,
        status: 'offline'
      });
    }
    return NextResponse.json({
      ...currentStatus,
      status: 'online'
    });
  } catch (error) {
    console.error('Error fetching status:', error);
    return NextResponse.json(
      { error: 'Failed to fetch status' },
      { status: 500 }
    );
  }
}

export async function OPTIONS() {
  return new NextResponse(null, {
    status: 200,
    headers: {
      'Access-Control-Allow-Origin': '*', // Adjust for production
      'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type, X-API-Key',
    },
  });
}
```

#### **2. Configure Your API Key (Next.js)**

*   **Generate a Strong API Key:** Create a unique string (e.g., `psk_yourRandomSecureKeyHere123!`).
*   **Set Environment Variable:**
    *   **Vercel (Recommended):** Go to your project's settings on Vercel -> Environment Variables, and add `PERSONAL_STATUS_API_KEY` with your generated key.
    *   **Local Development:** Create a `.env.local` file in your Next.js project root:
        ```
        PERSONAL_STATUS_API_KEY=psk_yourRandomSecureKeyHere123!
        ```
*   **Important:** This API key must match the one entered into the Personal Status Monitor desktop app.

#### **3. Deploy & Host (Next.js)**
*   Deploy (or re-deploy) your Next.js application.
*   **Hosting Platforms:** Vercel (highly recommended for Next.js), Netlify, AWS Amplify, Google Firebase Hosting (with Cloud Functions for backend), or other platforms supporting Next.js.

#### **4. How This API Route Works (Next.js Specifics):**
*   Next.js file-system routing automatically maps `app/api/status/route.ts` to the `/api/status` endpoint.
*   The exported `GET`, `POST`, `OPTIONS` functions handle the respective HTTP methods.
*   When deployed to Vercel, these often run as serverless functions.

---

### **Option 2: Node.js with Express.js**

This is a good option if you have an existing Node.js backend, prefer Express.js, or are deploying to a platform more suited for traditional Node.js servers.

#### **1. Create the Server File**

Create a file (e.g., `server.js` or `statusApi.js`) in your Node.js project:

```javascript
// filepath: server.js (or your chosen filename)
const express = require('express');
const cors = require('cors');
require('dotenv').config(); // For loading .env file

const app = express();
const port = process.env.PORT || 3001; // Use environment variable for port

// Your personal API key from environment variable
const API_KEY = process.env.PERSONAL_STATUS_API_KEY;

let currentStatus = {
  thoughts: "App offline",
  activeApps: [],
  busy: false,
  timestamp: 0,
  lastUpdated: null
};

// CORS Configuration
const corsOptions = {
  origin: '*', // For production, restrict to your portfolio's domain: 'https://your-portfolio-domain.com'
  methods: ['GET', 'POST', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'X-API-Key']
};
app.use(cors(corsOptions));
app.options('/api/status', cors(corsOptions)); // Enable pre-flight for /api/status

app.use(express.json()); // Middleware to parse JSON bodies

function validateApiKey(req) {
  const apiKeyFromHeader = req.headers['x-api-key']; // Headers are often lowercased by servers
  return apiKeyFromHeader === API_KEY;
}

// POST endpoint to update status
app.post('/api/status', (req, res) => {
  if (!API_KEY) {
    console.error('API Key not configured on the server.');
    return res.status(500).json({ error: 'Server configuration error: API Key missing' });
  }
  if (!validateApiKey(req)) {
    return res.status(401).json({ error: 'Unauthorized - Invalid API key' });
  }

  const data = req.body;
  if (typeof data.thoughts === 'string' &&
      Array.isArray(data.activeApps) &&
      typeof data.busy === 'boolean' &&
      typeof data.timestamp === 'number') {
    currentStatus = { ...data, lastUpdated: new Date() };
    console.log('Status updated:', currentStatus);
    return res.json({ success: true, message: 'Status updated successfully' });
  } else {
    return res.status(400).json({ error: 'Invalid data format' });
  }
});

// GET endpoint to fetch status
app.get('/api/status', (req, res) => {
  const now = Date.now() / 1000;
  const isRecent = currentStatus.timestamp && (now - currentStatus.timestamp) < 15;

  if (!isRecent || currentStatus.thoughts === "App offline") {
    return res.json({
      thoughts: "App offline",
      activeApps: [],
      busy: false,
      timestamp: 0,
      lastUpdated: null,
      status: 'offline'
    });
  }
  return res.json({ ...currentStatus, status: 'online' });
});

app.listen(port, () => {
  console.log(`Personal Status API server listening on port ${port}`);
  if (!API_KEY) {
    console.warn('Warning: PERSONAL_STATUS_API_KEY is not set. POST requests will fail.');
  }
});
```

#### **2. Install Dependencies (Node.js/Express.js)**
In your Node.js project directory, install the necessary packages:
```bash
npm install express cors dotenv
# or
yarn add express cors dotenv
```

#### **3. Configure Your API Key (Node.js/Express.js)**
*   **Generate a Strong API Key:** Create a unique string (e.g., `psk_yourRandomSecureKeyHere123!`).
*   **Set Environment Variable:**
    *   Create a `.env` file in your Node.js project root (ensure `.env` is in your `.gitignore`):
        ```
        PERSONAL_STATUS_API_KEY=psk_yourRandomSecureKeyHere123!
        PORT=3001 # Optional: specify a port
        ```
    *   When deploying, set `PERSONAL_STATUS_API_KEY` (and `PORT` if needed) as environment variables on your hosting platform.
*   **Important:** This API key must match the one entered into the Personal Status Monitor desktop app.

#### **4. Run and Deploy (Node.js/Express.js)**
*   **Local Development:** Run `node server.js`.
*   **Deployment & Hosting Platforms:**
    *   Heroku, Render, Railway
    *   AWS (EC2, Elastic Beanstalk, Fargate, Lambda with API Gateway)
    *   Google Cloud (App Engine, Cloud Run, Compute Engine)
    *   DigitalOcean (App Platform, Droplets)
    *   Azure (App Service, Functions)
    *   Fly.io
    *   Ensure your hosting platform allows you to set environment variables and runs your Node.js application (e.g., via a `package.json` start script: `"start": "node server.js"`).

---

### **Common Steps After Backend Setup**

#### **1. Configure the Desktop App**
No matter which backend option you choose:
*   When you run the Personal Status Monitor desktop application, in the setup dialog (or settings):
    *   Enter your portfolio's full API endpoint URL (e.g., `https://your-portfolio-domain.com/api/status` or `http://localhost:3001/api/status` for local Express testing).
    *   Enter the same API key you configured for your chosen backend.

#### **2. How The API Logic Works (General)**
*   **`POST /api/status`**: Used by the desktop app to send status updates. Requires `X-API-Key` header.
*   **`GET /api/status`**: Publicly fetches current status for your portfolio frontend.
*   **`OPTIONS /api/status`**: Handles CORS preflight requests.

With your chosen backend setup, your desktop application will be able to securely update your portfolio website!

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

**Test for your Portfolio Website:**
```bash
# Should return JSON status
curl https://your-saved-api-url.com/api/status

# Or visit in browser
https://your-saved-api-url.com/api/status
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

## Architecture

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
              │  POST /api/status ( API Key Required)               │
              │  • Updates current status                           │
              │  • Validates authentication                         │
              │  • Stores in memory                                 │
              │                                                     │
              │  GET /api/status (Public Access)                    │
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
┌─────────────────┐    API Key        ┌─────────────────┐
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

- ✅ **Conditional Compilation** - `#ifdef` for platform code
- ✅ **CMake** - Modern build system
