#ifdef _WIN32  // Only compile on Windows

#include "SystemTray_Win32.h"
#include <iostream>
#include <commctrl.h>

SystemTray_Win32::SystemTray_Win32() 
    : hwnd(nullptr), hMenu(nullptr), isCreated(false) {
    // Initialize NOTIFYICONDATA structure
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
}

SystemTray_Win32::~SystemTray_Win32() {
    if (isCreated) {
        // Remove tray icon
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }
    
    if (hMenu) {
        DestroyMenu(hMenu);
    }
    
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

bool SystemTray_Win32::create() {
    // Create hidden window to handle tray messages
    if (!createHiddenWindow()) {
        std::cerr << "Failed to create hidden window for system tray" << std::endl;
        return false;
    }
    
    // Set up tray icon data
    nid.hWnd = hwnd;
    nid.uID = TRAY_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    
    // Load default icon (you can customize this)
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    
    // Set default tooltip
    wcscpy_s(nid.szTip, L"Personal Status Monitor");
    
    // Add icon to system tray
    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        std::cerr << "Failed to add icon to system tray" << std::endl;
        return false;
    }
    
    isCreated = true;
    createContextMenu();
    
    std::cout << "System tray icon created successfully" << std::endl;
    return true;
}

void SystemTray_Win32::show() {
    if (isCreated) {
        Shell_NotifyIcon(NIM_ADD, &nid);
    }
}

void SystemTray_Win32::hide() {
    if (isCreated) {
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }
}

void SystemTray_Win32::setTooltip(const std::string& tooltip) {
    if (!isCreated) return;
    
    std::wstring wtooltip = stringToWstring(tooltip);
    wcscpy_s(nid.szTip, wtooltip.c_str());
    nid.uFlags = NIF_TIP;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; // Restore all flags
}

void SystemTray_Win32::showNotification(const std::string& title, const std::string& message) {
    if (!isCreated) return;
    
    // Set up balloon notification
    nid.uFlags |= NIF_INFO;
    wcscpy_s(nid.szInfoTitle, stringToWstring(title).c_str());
    wcscpy_s(nid.szInfo, stringToWstring(message).c_str());
    nid.dwInfoFlags = NIIF_INFO;
    nid.uTimeout = 3000; // 3 seconds
    
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    
    // Restore original flags
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
}

void SystemTray_Win32::addMenuItem(const std::string& text, int id) {
    MenuItem item;
    item.text = text;
    item.id = id;
    item.isSeparator = false;
    menuItems.push_back(item);
    
    // Recreate menu if it already exists
    if (hMenu) {
        createContextMenu();
    }
}

void SystemTray_Win32::addSeparator() {
    MenuItem item;
    item.text = "";
    item.id = 0;
    item.isSeparator = true;
    menuItems.push_back(item);
    
    // Recreate menu if it already exists
    if (hMenu) {
        createContextMenu();
    }
}

bool SystemTray_Win32::createHiddenWindow() {
    // Register window class
    const wchar_t* className = L"PersonalStatusTrayWindow";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = className;
    
    if (!RegisterClassW(&wc)) {
        // Class might already be registered
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }
    
    // Create hidden window
    hwnd = CreateWindowW(
        className,
        L"Personal Status Tray",
        0,  // No style - hidden window
        0, 0, 0, 0,  // Position and size (hidden)
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this  // Pass 'this' pointer
    );
    
    return hwnd != nullptr;
}

void SystemTray_Win32::createContextMenu() {
    if (hMenu) {
        DestroyMenu(hMenu);
    }
    
    hMenu = CreatePopupMenu();
    
    for (const auto& item : menuItems) {
        if (item.isSeparator) {
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        } else {
            std::wstring wtext = stringToWstring(item.text);
            AppendMenuW(hMenu, MF_STRING, item.id, wtext.c_str());
        }
    }
}

void SystemTray_Win32::showContextMenu() {
    if (!hMenu || !hwnd) return;
    
    POINT pt;
    GetCursorPos(&pt);
    
    // Required for proper menu behavior
    SetForegroundWindow(hwnd);
    
    // Show menu and get selected item
    int cmd = TrackPopupMenu(hMenu, 
                           TPM_RETURNCMD | TPM_RIGHTBUTTON, 
                           pt.x, pt.y, 
                           0, hwnd, nullptr);
    
    // Handle menu selection
    if (cmd > 0 && onMenuItemClicked) {
        onMenuItemClicked(cmd);
    }
    
    // Required cleanup
    PostMessage(hwnd, WM_NULL, 0, 0);
}

LRESULT CALLBACK SystemTray_Win32::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SystemTray_Win32* tray = nullptr;
    
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        tray = reinterpret_cast<SystemTray_Win32*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(tray));
    } else {
        tray = reinterpret_cast<SystemTray_Win32*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (tray) {
        switch (uMsg) {
            case WM_TRAYICON:
                switch (lParam) {
                    case WM_LBUTTONUP:
                        // Left click - call callback with special ID
                        if (tray->onMenuItemClicked) {
                            tray->onMenuItemClicked(-1); // -1 for left click
                        }
                        break;
                        
                    case WM_RBUTTONUP:
                        // Right click - show context menu
                        tray->showContextMenu();
                        break;
                }
                break;
                
            case WM_COMMAND:
                // Menu item selected
                if (tray->onMenuItemClicked) {
                    tray->onMenuItemClicked(LOWORD(wParam));
                }
                break;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::wstring SystemTray_Win32::stringToWstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

#endif // _WIN32