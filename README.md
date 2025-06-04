# Personal Status Monitor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)

A real-time desktop widget that shows your current thoughts and active applications on your portfolio website.

## 🎯 What It Does

This app creates a desktop widget that:
- 📝 Tracks your current thoughts (typed in real-time)
- 🟢 Monitors your busy/free status 
- 🖥️ Detects which development apps you're using
- 🚀 Pushes this data to your portfolio website every 2 seconds

## 📸 Example Output

### Desktop Widget
When you run the app, you'll see a small overlay window where you can:
- Type your current thoughts
- Toggle between "Free" and "Busy" status
- The window stays on your desktop and updates your portfolio in real-time

### Console Output
```
Personal Status Monitor - Desktop Widget
========================================
Monitoring: Brave, VS Code, PowerShell, Android Studio, Docker, Postman, Visual Studio
Local Server: http://localhost:8081
Vercel API: Pushing every 2 seconds (Secured with API key)
Press Ctrl+C to exit

Configuration loaded from .env file
API URL: https://my-portfolio.vercel.app/api/status

Starting local web server...
Web server started on port 8081
Creating overlay window...
Starting Vercel API push loop...
All components started. GUI running...

[VERCEL] ✓ Sent: {"timestamp":1701234567,"thoughts":"Working on a new React component","activeApps":["Visual Studio Code","Brave Browser"],"busy":false}
[VERCEL] ✓ Sent: {"timestamp":1701234569,"thoughts":"Debugging API integration","activeApps":["Visual Studio Code","Postman"],"busy":true}
[VERCEL] ✓ Sent: {"timestamp":1701234571,"thoughts":"Taking a break","activeApps":[],"busy":false}
```

### Portfolio Website Display
Your website visitors will see something like:

```
🟢 Online

Current Thoughts: "Working on a new React component"
Availability: 🟢 Available
Currently Using: Visual Studio Code, Brave Browser
Last updated: 2:34:27 PM
```

### API Response Example
Your `/api/status` endpoint returns:
```json
{
  "thoughts": "Working on a new React component",
  "activeApps": ["Visual Studio Code", "Brave Browser"],
  "busy": false,
  "timestamp": 1701234567,
  "lastUpdated": "2023-11-29T14:34:27.000Z",
  "status": "online"
}
```

## Setup Instructions

### Prerequisites
- Windows 10/11
- MinGW/GCC compiler
- Next.js portfolio website
- Vercel account (free tier works)

### 1. Clone and Configure

```bash
git clone https://github.com/yourusername/personal-status-monitor
cd personal-status-monitor

# Copy the example environment file
cp .env.example .env

# Edit .env with your values
notepad .env
```

### 2. Deploy Your Vercel API

1. **Add the API route to your Next.js portfolio:**
   ```typescript
   // app/api/status/route.ts
   import { NextRequest, NextResponse } from 'next/server';

   const API_KEY = process.env.PERSONAL_STATUS_API_KEY;
   let currentStatus = {
     thoughts: "App offline",
     activeApps: [],
     busy: false,
     timestamp: 0,
     lastUpdated: null as Date | null
   };

   function validateApiKey(request: NextRequest): boolean {
     const apiKeyFromHeader = request.headers.get('X-API-Key');
     return apiKeyFromHeader === API_KEY;
   }

   export async function POST(request: NextRequest) {
     if (!validateApiKey(request)) {
       return NextResponse.json({ error: 'Unauthorized' }, { status: 401 });
     }

     const data = await request.json();
     currentStatus = { ...data, lastUpdated: new Date() };
     return NextResponse.json({ success: true });
   }

   export async function GET() {
     const now = Date.now() / 1000;
     const isRecent = currentStatus.timestamp && (now - currentStatus.timestamp) < 10;
     
     if (!isRecent) {
       return NextResponse.json({
         thoughts: "App offline",
         activeApps: [],
         busy: false,
         timestamp: 0,
         status: 'offline'
       });
     }
     
     return NextResponse.json({ ...currentStatus, status: 'online' });
   }
   ```

2. **Deploy to Vercel and set environment variable:**
   ```bash
   vercel
   # In Vercel Dashboard: Add PERSONAL_STATUS_API_KEY environment variable
   ```

3. **Update your .env file:**
   ```bash
   VERCEL_API_URL=https://your-actual-deployment.vercel.app/api/status
   API_KEY=your-secret-key-from-vercel
   ```

### 3. Build and Run

```bash
# Build the application
g++ -g -std=c++17 -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601 main.cpp AppDetector.cpp WebServer.cpp OverlayWindow.cpp ThoughtsManager.cpp -o overlay_app.exe -lwinhttp -lws2_32 -lgdi32 -luser32 -lcomctl32

# Run the application
./overlay_app.exe
```

## ✨ Features

- ✅ **Real-time thoughts sharing** - Type what you're thinking, visitors see it instantly
- ✅ **Availability status** - Toggle between busy/free
- ✅ **App monitoring** - Automatically detects what development tools you're using
- ✅ **Secure API** - Protected with API key authentication
- ✅ **Local web server** - Also serves data locally on port 8081
- ✅ **Desktop widget** - Draggable, semi-transparent overlay window
- ✅ **Environment configuration** - Easy setup with `.env` files
- ✅ **Cross-platform friendly** - Designed for easy porting

## 🔒 Security

- Your API key is private and stored in `.env` (not committed to git)
- Only POST requests (updating status) require authentication
- GET requests (viewing status) are public for portfolio visitors
- Each person uses their own Vercel deployment and API key

## 🎯 Monitored Applications

- Brave Browser (`brave.exe`)
- Visual Studio Code (`Code.exe`) 
- PowerShell (`powershell.exe`)
- Android Studio (`studio64.exe`)
- Docker Desktop (`Docker Desktop.exe`)
- Postman (`Postman.exe`)
- Visual Studio (`devenv.exe`)

Want to monitor different apps? Customize the list in [`AppDetector.cpp`](AppDetector.cpp).



## 🤝 Contributing

We welcome contributions! Here's how you can help:

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```
3. **Make your changes**
4. **Test thoroughly**
5. **Commit your changes**
   ```bash
   git commit -m 'Add some amazing feature'
   ```
6. **Push to the branch**
   ```bash
   git push origin feature/amazing-feature
   ```
7. **Open a Pull Request**


⭐ **Star this repo if you find it useful!**