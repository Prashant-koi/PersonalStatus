#ifdef _WIN32

#include "SystemTray_Win32.h"
#include <iostream>
#include <commctrl.h>

SystemTray_Win32 :: SystemTray_Win32()
    : hwnd(nullptr), hMenu(nullptr), isCreated(false) {
        //initializw
        ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
        nid.cbSize = sizeof(NOTIFYICONDATA);
}

SystemTray_Win32::~SystemTray_Win32() {
    if (isCreated) {
        //remore tray icon
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }

    if (hMenu) {
        DestroyMenu(hMenu);
    }

    if (hwnd) {
        DestroyWindow(hwnd);
    }
}



#endif