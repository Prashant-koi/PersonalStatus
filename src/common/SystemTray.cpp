#include "SystemTray.h"

#ifdef _WIN32
    #include "../windows/SystemTray_Win32.h"
#endif

// Factory method - Windows only for v0.1.0
SystemTray* SystemTray::createPlatformTray() {
    #ifdef _WIN32
        return new SystemTray_Win32();
    #else
        #error "This version only supports Windows. Linux support coming in v0.2.0"
    #endif
}