#ifdef _WIN32 //Only Windows

#include "OverlayWindow_Win32.h"
#include "../common/SettingsManager.h"
#include "../windows/SetupDialog_Win32.h"
#include <commctrl.h>
#include <iostream>
#include <windows.h>

#pragma comment(lib, "comctl32.lib")

// Control IDs - consistent with resource.h
#define ID_EDIT_THOUGHTS 1001
#define ID_TOGGLE_BUSY 1002
#define ID_SETTINGS_BUTTON 1006  // NEW: Use ID from resource.h

OverlayWindow_Win32::OverlayWindow_Win32() 
    : hwnd(nullptr), hEditThoughts(nullptr), hToggleBusy(nullptr), 
      hSettingsButton(nullptr), hLabelThoughts(nullptr), hLabelBusy(nullptr), 
      hFont(nullptr), thoughtsManager(nullptr), isBusyToggled(false) {
}

OverlayWindow_Win32::~OverlayWindow_Win32() {
    if (hFont) {
        DeleteObject(hFont);  // Clean up font
    }
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

void OverlayWindow_Win32::setThoughtsManager(ThoughtsManager* manager) {
    thoughtsManager = manager;
}

bool OverlayWindow_Win32::create() {
    // Register window class - Use TEXT macro for proper string literals
    const TCHAR* className = TEXT("PersonalStatusOverlay");
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30)); // Dark background
    
    if (!RegisterClass(&wc)) {
        return false;
    }
    
    // Get screen dimensions for bottom positioning
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    int windowWidth = 300;
    int windowHeight = 150;
    
    // Position at bottom-right corner
    int xPos = screenWidth - windowWidth - 20;
    int yPos = screenHeight - windowHeight - 50; // Leave space for taskbar
    
    // Create the window - Use TEXT macro, remove TOPMOST for desktop level
    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW,  // Layered for transparency, toolwindow to not show in taskbar
        className,
        TEXT("Personal Status"),
        WS_POPUP | WS_VISIBLE | WS_CAPTION,  // Popup window with caption for moving
        xPos,           // x position (bottom-right)
        yPos,           // y position  
        windowWidth,    // width
        windowHeight,   // height
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this    // Pass 'this' pointer to WM_CREATE
    );
    
    if (!hwnd) {
        return false;
    }
    
    setupWindowStyle();
    createControls();
    
    return true;
}

void OverlayWindow_Win32::setupWindowStyle() {
    // Make window semi-transparent
    SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA); // 200/255 opacity
    
    // Set window to stay above desktop but below normal windows
    // Remove the desktop parenting to make it visible
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    // Comment out desktop parenting for now - this was hiding the window
    /*
    HWND progman = FindWindow(TEXT("Progman"), nullptr);
    if (progman) {
        SetParent(hwnd, progman);
    }
    */
}

void OverlayWindow_Win32::createControls() {
    // "Your current thoughts" label - Use TEXT macro
    hLabelThoughts = CreateWindow(
        TEXT("STATIC"),
        TEXT("Your current thoughts:"),
        WS_VISIBLE | WS_CHILD,
        10, 10, 280, 20,
        hwnd,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Thoughts input box - Use TEXT macro
    hEditThoughts = CreateWindow(
        TEXT("EDIT"),
        TEXT(""),
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        10, 35, 280, 60,
        hwnd,
        (HMENU)ID_EDIT_THOUGHTS,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Busy/Free toggle button - Use TEXT macro
    hToggleBusy = CreateWindow(
        TEXT("BUTTON"),
        TEXT("Free"),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        10, 105, 80, 30,
        hwnd,
        (HMENU)ID_TOGGLE_BUSY,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Settings button - next to Free/Busy button
    hSettingsButton = CreateWindow(
        TEXT("BUTTON"),
        TEXT("Settings"),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        100, 105, 80, 30,  // Position next to Free/Busy button
        hwnd,
        (HMENU)ID_SETTINGS_BUTTON,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Set font for better visibility - Use TEXT macro
    hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                            CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));
    
    SendMessage(hLabelThoughts, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEditThoughts, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hToggleBusy, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hSettingsButton, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void OverlayWindow_Win32::show() {
    if (hwnd) {
        // First, bring window to topmost
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, 
                    SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        
        // Then show the window
        ShowWindow(hwnd, SW_SHOW);
        
        // Force it to the foreground
        SetForegroundWindow(hwnd);
        
        // Ensure it gets focus
        SetActiveWindow(hwnd);
        
        // Update the window
        UpdateWindow(hwnd);
        
        // Optional: Flash the window to draw attention
        FlashWindow(hwnd, TRUE);
        
        std::cout << "[WINDOW] Shown and brought to top" << std::endl;
    }
}

void OverlayWindow_Win32::hide() {
    if (hwnd) {
        // Remove topmost flag when hiding
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, 
                    SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
        
        std::cout << "[WINDOW] Hidden" << std::endl;
    }
}

void OverlayWindow_Win32::messageLoop() {
    MSG msg;
    // Process messages for ALL windows in this thread
    while (GetMessage(&msg, nullptr, 0, 0)) {  // nullptr = all windows
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void OverlayWindow_Win32::updateThoughts() {
    if (!thoughtsManager || !hEditThoughts) return;
    
    // Get text from edit control
    int length = GetWindowTextLength(hEditThoughts);
    if (length > 0) {
        TCHAR* buffer = new TCHAR[length + 1];
        GetWindowText(hEditThoughts, buffer, length + 1);
        
        // Convert to string and update thoughts manager
        #ifdef UNICODE
            std::wstring wtext(buffer);
            std::string text(wtext.begin(), wtext.end());
        #else
            std::string text(buffer);
        #endif
        thoughtsManager->setCurrentThoughts(text);
        
        delete[] buffer;
    } else {
        thoughtsManager->setCurrentThoughts("");
    }
}

void OverlayWindow_Win32::toggleBusyStatus() {
    if (!thoughtsManager) return;
    
    isBusyToggled = !isBusyToggled;
    thoughtsManager->setBusyStatus(isBusyToggled);
    
    // Update button text - Use TEXT macro
    SetWindowText(hToggleBusy, isBusyToggled ? TEXT("Busy") : TEXT("Free"));
}

void OverlayWindow_Win32::showSettingsDialog() {
    std::string endpoint, apiKey;
    SettingsManager::loadSettings(endpoint, apiKey);
    
    if (SetupDialog_Win32::showSettingsDialog(endpoint, apiKey)) {
        // Save new settings
        if (SettingsManager::saveSettings(endpoint, apiKey)) {
            MessageBox(hwnd, L"Settings saved successfully!\nRestart the application for changes to take effect.", 
                      L"Settings Updated", MB_ICONINFORMATION | MB_OK);
        } else {
            MessageBox(hwnd, L"Failed to save settings.", L"Error", MB_ICONERROR | MB_OK);
        }
    }
}

LRESULT CALLBACK OverlayWindow_Win32::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    OverlayWindow_Win32* window = nullptr;
    
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<OverlayWindow_Win32*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<OverlayWindow_Win32*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (window) {
        switch (uMsg) {
            case WM_NCHITTEST:
                {
                    // Make the entire window draggable (like a title bar)
                    LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
                    if (hit == HTCLIENT) {
                        return HTCAPTION; // Treat client area as title bar for dragging
                    }
                    return hit;
                }
                break;
                
            case WM_COMMAND:
                if (LOWORD(wParam) == ID_EDIT_THOUGHTS && HIWORD(wParam) == EN_CHANGE) {
                    window->updateThoughts();
                } else if (LOWORD(wParam) == ID_TOGGLE_BUSY && HIWORD(wParam) == BN_CLICKED) {
                    window->toggleBusyStatus();
                } else if (LOWORD(wParam) == ID_SETTINGS_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                    window->showSettingsDialog();  // New method
                }
                break;
                
            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLOREDIT:
                {
                    HDC hdc = (HDC)wParam;
                    SetTextColor(hdc, RGB(255, 255, 255)); // White text
                    SetBkColor(hdc, RGB(40, 40, 40));      // Dark background
                    return (LRESULT)CreateSolidBrush(RGB(40, 40, 40));
                }
                
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif //_WIN32