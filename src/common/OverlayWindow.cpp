#include "OverlayWindow.h"

#ifdef _WIN32
    #include "../windows/OverlayWindow_Win32.h"
#elif __linux__
    #include "../linux/OverlayWindow_Wayland.h"
#endif

// Factory method - creates platform-specific window
OverlayWindow* OverlayWindow::createPlatformWindow() {
    #ifdef _WIN32
        return new OverlayWindow_Win32();
    #elif __linux__
        return new OverlayWindow_Wayland();
    #else
        #error "Unsupported platform"
    #endif
}