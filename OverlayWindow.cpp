#include "OverlayWindow.h"
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
    // Register window class
    const wchar_t* className = L"PersonalStatusOverlay";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30)); // Dark background
    
    if (!RegisterClass(&wc)) {
        return false;
    }
    
    // Create the window
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,  // Always on top, layered, no taskbar
        className,
        L"Personal Status",
        WS_POPUP | WS_VISIBLE,  // Popup window, no borders
        50,     // x position
        50,     // y position  
        300,    // width
        150,    // height
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
    
    // Set window to stay on top
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void OverlayWindow::createControls() {
    // "Your current thoughts" label
    hLabelThoughts = CreateWindow(
        L"STATIC",
        L"Your current thoughts:",
        WS_VISIBLE | WS_CHILD,
        10, 10, 280, 20,
        hwnd,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Thoughts input box
    hEditThoughts = CreateWindow(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        10, 35, 280, 60,
        hwnd,
        (HMENU)ID_EDIT_THOUGHTS,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Busy/Free toggle button
    hToggleBusy = CreateWindow(
        L"BUTTON",
        L"Free",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        10, 105, 80, 30,
        hwnd,
        (HMENU)ID_TOGGLE_BUSY,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    // Set dark theme colors
    SendMessage(hLabelThoughts, WM_CTLCOLORSTATIC, 0, 0);
    
    // Set font for better visibility
    HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                            CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    
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
        wchar_t* buffer = new wchar_t[length + 1];
        GetWindowText(hEditThoughts, buffer, length + 1);
        
        // Convert to string and update thoughts manager
        std::wstring wtext(buffer);
        std::string text(wtext.begin(), wtext.end());
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
    
    // Update button text
    SetWindowText(hToggleBusy, isBusyToggled ? L"Busy" : L"Free");
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