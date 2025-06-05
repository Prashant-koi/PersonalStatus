#include"OverlayWindow.h"

//platform specific heaaders
#ifdef _WIN32
    #include "../windows/OverlayWindow_Win32.h"
#elif __linux__
    #include "../linux/OverlayWindow_X11.h"
#endif


// factory method for platform specific implementation

OverlayWindow* OverlayWindow :: createPlatformWindow() {
    #ifdef _WIN32
        return new OverlayWindow_Win32();
    #elif __linux__
        return new OverlayWindow_X11();
    #else
        #error "Unsupported platform"
    #endif
}