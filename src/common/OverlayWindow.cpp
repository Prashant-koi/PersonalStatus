#include "OverlayWindow.h"

#ifdef _WIN32
    #include "../windows/OverlayWindow_Win32.h"
#endif

// Factory method - Windows only for v0.1.0
OverlayWindow* OverlayWindow::createPlatformWindow() {
    #ifdef _WIN32
        return new OverlayWindow_Win32();
    #else
        #error "This version only supports Windows. Linux support coming in v0.2.0"
    #endif
}