# Personal Status Monitor - Hyprland/Wayland Edition

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux-blue.svg)](https://github.com/Prashant-koi/PersonalStatus)
[![Language](https://img.shields.io/badge/language-C++17-blue.svg)](https://isocpp.org/)
[![Desktop Environment](https://img.shields.io/badge/DE-Hyprland%2FWayland-purple.svg)](https://hyprland.org/)

A real-time desktop widget for Linux that shows your current thoughts and active applications on your portfolio website.

> **✨ This is the Linux/Hyprland implementation** featuring native Wayland support with GTK-layer-shell!

## 🌟 Features

### **🖥️ Desktop Integration**
- **Native Wayland Support**: Built with GTK3 + gtk-layer-shell for seamless Hyprland integration
- **Floating Overlay Widget**: Semi-transparent window positioned in top-right corner
- **System Tray Integration**: Lives in your system tray with full control options
- **Hotkey Support**: Toggle visibility with `pkill -f -USR1 personal-status-monitor`
- **Auto-Start**: Starts automatically with your desktop session

### **📡 Real-Time Portfolio Integration**
- **Live Thoughts Sharing**: Type what you're thinking, visitors see it instantly on your portfolio
- **Availability Status**: Toggle between "🟢 Available" and "🔴 Busy" states
- **Application Detection**: Automatically detects and displays active applications (VS Code, browsers, etc.)
- **Smart API Updates**: Only sends data when something changes (efficient networking)
- **Secure Authentication**: Uses API key authentication for portfolio integration

### **⚙️ User Experience**
- **Hide/Show System**: Click "—" button to hide, use hotkey or tray to show again
- **Real-Time Input**: Thoughts update as you type (with debouncing)
- **Focus Management**: Click-to-focus input field with proper keyboard handling
- **Persistent Settings**: Remembers your API configuration between sessions

## 🚀 Quick Start

### **Prerequisites**
- **Hyprland/Wayland** compositor
- **System tray** (waybar, eww, or similar)
- **GTK3** and **gtk-layer-shell** libraries

### **1. Install Dependencies**

**Arch Linux:**
```bash
sudo pacman -S gtk3 gtk-layer-shell curl cmake gcc
```

**Ubuntu/Debian:**
```bash
sudo apt install libgtk-3-dev libgtk-layer-shell-dev libcurl4-openssl-dev cmake build-essential
```

### **2. Build & Install**

```bash
# Clone the repository
git clone https://github.com/yourusername/PersonalStatus.git
cd PersonalStatus

# Build the application
chmod +x scripts/build.sh
./scripts/build.sh

# The executable will be created at: ./build/personal-status-monitor
```

### **3. First Run Setup**

```bash
# Run the application for initial setup
./build/personal-status-monitor

# Follow the setup dialog to configure:
# - Portfolio API Endpoint (e.g., https://your-portfolio.vercel.app/api/status)
# - API Key (e.g., psk_your_secure_random_key_here)
```

### **4. Enable Auto-Start (Optional)**

In the widget overlay:
1. Click **"Enable"** next to "🚀 Auto-start"
2. The app will start automatically with your desktop session (minimized to tray)

## 🎯 Usage

### **Widget Control**
- **Show Widget**: Right-click tray icon → "Show Status Widget"
- **Hide Widget**: Click "—" button in widget, or right-click tray → "Hide Status Widget"
- **Toggle Widget**: `pkill -f -USR1 personal-status-monitor` (can be bound to hotkey)
- **Exit App**: Right-click tray icon → "Exit Application"

### **Status Updates**
- **Type Thoughts**: Click in text field and type what you're working on
- **Toggle Status**: Click "Toggle Status" to switch between Available/Busy
- **Live Updates**: Changes are sent to your portfolio in real-time

### **Hotkey Integration**

Add to your Hyprland config (`~/.config/hypr/hyprland.conf`):
```conf
# Toggle Personal Status Monitor widget
bind = SUPER, F12, exec, pkill -f -USR1 personal-status-monitor
```

Or create a custom script:
```bash
echo '#!/bin/bash
pkill -f -USR1 personal-status-monitor' > ~/.local/bin/toggle-status-widget
chmod +x ~/.local/bin/toggle-status-widget

# Then bind it in your compositor/hotkey manager
```

## 🌐 Portfolio Integration

### **Backend Setup (Required)**

Your portfolio website needs an API endpoint to receive status updates:

**Example API Route (`/api/status`):**
```javascript
// pages/api/status.js (Next.js example)
export default async function handler(req, res) {
  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Method not allowed' });
  }

  // Verify API key
  const apiKey = req.headers['x-api-key'];
  if (apiKey !== process.env.PERSONAL_STATUS_API_KEY) {
    return res.status(401).json({ error: 'Unauthorized' });
  }

  // Process status update
  const { timestamp, thoughts, activeApps, busy } = req.body;
  
  // Store in database or state management system
  // Update your portfolio display
  
  res.status(200).json({ success: true });
}
```

### **Frontend Display**

Display the status on your portfolio:
```jsx
// components/StatusWidget.jsx
export function StatusWidget({ status }) {
  return (
    <div className="status-widget">
      <div className={`availability ${status.busy ? 'busy' : 'available'}`}>
        {status.busy ? '🔴 Busy' : '🟢 Available'}
      </div>
      
      <div className="thoughts">
        💭 {status.thoughts || 'Just working on some cool stuff...'}
      </div>
      
      <div className="apps">
        📱 Currently using: {status.activeApps?.join(', ') || 'Various tools'}
      </div>
      
      <div className="timestamp">
        Last updated: {new Date(status.timestamp * 1000).toLocaleTimeString()}
      </div>
    </div>
  );
}
```

## 🛠️ Advanced Configuration

### **Application Location**

After building, you can move the executable anywhere:
```bash
# Install system-wide (optional)
sudo cp build/personal-status-monitor /usr/local/bin/
sudo chmod +x /usr/local/bin/personal-status-monitor

# Or keep it local
mkdir -p ~/.local/bin
cp build/personal-status-monitor ~/.local/bin/
```

### **Desktop Entry (Manual)**

If auto-start doesn't work automatically, create manually:
```bash
mkdir -p ~/.local/share/applications

cat > ~/.local/share/applications/personal-status-monitor.desktop << 'EOF'
[Desktop Entry]
Version=1.0
Type=Application
Name=Personal Status Monitor
Comment=Desktop status widget for portfolio integration
GenericName=Status Widget
Exec=/path/to/your/personal-status-monitor
Icon=applications-system
Terminal=false
StartupNotify=false
Categories=Utility;System;Monitor;
Keywords=status;portfolio;widget;monitor;
StartupWMClass=personal-status-monitor
EOF

# Make it executable
chmod +x ~/.local/share/applications/personal-status-monitor.desktop

# Update desktop database
update-desktop-database ~/.local/share/applications/
```

### **Autostart Entry**
```bash
mkdir -p ~/.config/autostart

cat > ~/.config/autostart/personal-status-monitor.desktop << 'EOF'
[Desktop Entry]
Type=Application
Name=Personal Status Monitor
Comment=Desktop status widget for portfolio integration
Exec=/path/to/your/personal-status-monitor --minimized
Icon=applications-system
Terminal=false
Hidden=false
X-GNOME-Autostart-enabled=true
StartupNotify=false
Categories=Utility;System;
EOF
```

## 🎨 Customization

### **Widget Appearance**
The widget uses CSS styling that can be modified in the source code:
- **Colors**: Semi-transparent dark theme with gradients
- **Position**: Top-right corner with 20px margins
- **Size**: Auto-sized based on content

### **Keyboard Layer**
The widget uses `GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND` for proper focus handling in Wayland.

## 🔧 Troubleshooting

### **Widget Not Showing**
```bash
# Check if app is running
ps aux | grep personal-status-monitor

# Check for errors
./build/personal-status-monitor

# Toggle visibility
pkill -f -USR1 personal-status-monitor
```

### **Can't Type in Input Field**
```bash
# Restart the application
pkill personal-status-monitor
./build/personal-status-monitor
```

### **System Tray Not Visible**
Make sure your Wayland compositor supports system tray:
- **Waybar**: Add `"tray"` to your modules
- **Eww**: Configure tray widget
- **Swaybar**: Should work by default

### **API Not Working**
```bash
# Check API endpoint and key in setup
# Look for console output:
# [API] ✓ Data sent successfully
# [API] ✗ Failed to send data
```

## 📦 Building from Source

### **Dependencies**
```bash
# Build dependencies
sudo pacman -S cmake gcc gtk3 gtk-layer-shell curl pkgconf

# Runtime dependencies  
sudo pacman -S gtk3 gtk-layer-shell curl
```

### **Build Process**
```bash
git clone https://github.com/yourusername/PersonalStatus.git
cd PersonalStatus

# Configure and build
./scripts/build.sh

# Or manually:
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 🏗️ Architecture

### **Component Overview**
```
┌─────────────────────────────────────────────────────────────────┐
│                    Personal Status Monitor                      │
│                       Linux/Wayland Edition                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐    ┌──────────────────┐    ┌─────────────┐ │
│  │  GTK3 Overlay   │◄──►│   System Tray    │◄──►│ Config Mgr  │ │
│  │ + Layer Shell   │    │   Integration    │    │ (File-based)│ │
│  │                 │    │                  │    │             │ │
│  │ Real-time Input │    │ Show/Hide/Exit   │    │ Persistent  │ │
│  │ Status Toggle   │    │ Menu Options     │    │ Settings    │ │
│  └─────────────────┘    └──────────────────┘    └─────────────┘ │
│           │                       │                       │     │
│           ▼                       ▼                       ▼     │
│  ┌─────────────────┐    ┌──────────────────┐    ┌─────────────┐ │
│  │ Thoughts Manager│    │  App Detector    │    │ HTTP Client │ │
│  │ (Thread Safe)   │    │ (/proc scanner)  │    │ (libcurl)   │ │
│  │ • Text Storage  │    │ • Process Names  │    │ • POST JSON │ │  
│  │ • Status Toggle │    │ • Real-time Poll │    │ • API Auth  │ │
│  │ • Debouncing    │    │ • Smart Filter   │    │ • Error Log │ │
│  └─────────────────┘    └──────────────────┘    └─────────────┘ │
└─────────────────┬───────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Portfolio API Endpoint                      │
│                                                                 │
│  POST /api/status                                              │
│  Headers: X-API-Key: your_key_here                            │
│  Body: {                                                       │
│    "timestamp": 1635789123,                                    │
│    "thoughts": "Building awesome Wayland apps",               │
│    "activeApps": ["VS Code", "Firefox"],                      │
│    "busy": false                                               │
│  }                                                             │
└─────────────────────────────────────────────────────────────────┘
```

### **Key Technologies**
- **GTK3 + gtk-layer-shell**: Native Wayland desktop integration  
- **libcurl**: HTTP client for API communication
- **std::thread**: Asynchronous API updates and app detection
- **CMake**: Cross-platform build system
- **C++17**: Modern C++ features with RAII and smart pointers

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


