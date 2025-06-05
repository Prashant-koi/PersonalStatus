#ifdef _WIN32 //Only Windows

#include "OverlayWindow_Win32.h"
#include <commctrl.h>
#include <iostream>

#pragma comment(lib, "comctl32.lib")

#define ID_EDIT_THOUGHTS 1001
#define ID_TOGGLE_BUSY 1002

OverlayWindow::OverlayWindow() 
    : hwnd(nullptr), hEditThoughts(nullptr), hToggleBusy(nullptr), 
      hLabelThoughts(nullptr), hLabelBusy(nullptr), 
      thoughtsManager(nullptr), isBusyToggled(false) {
}

OverlayWindow::~OverlayWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

void OverlayWindow::setThoughtsManager(ThoughtsManager* manager) {
    thoughtsManager = manager;
}

bool OverlayWindow::create() {
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

void OverlayWindow::setupWindowStyle() {
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

void OverlayWindow::createControls() {
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
    
    // Set font for better visibility - Use TEXT macro
    HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                            CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));
    
    SendMessage(hLabelThoughts, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEditThoughts, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hToggleBusy, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void OverlayWindow::show() {
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}

void OverlayWindow::hide() {
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
}

void OverlayWindow::messageLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void OverlayWindow::updateThoughts() {
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

void OverlayWindow::toggleBusyStatus() {
    if (!thoughtsManager) return;
    
    isBusyToggled = !isBusyToggled;
    thoughtsManager->setBusyStatus(isBusyToggled);
    
    // Update button text - Use TEXT macro
    SetWindowText(hToggleBusy, isBusyToggled ? TEXT("Busy") : TEXT("Free"));
}

LRESULT CALLBACK OverlayWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    OverlayWindow* window = nullptr;
    
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<OverlayWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<OverlayWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
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