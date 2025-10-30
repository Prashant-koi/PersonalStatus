#include "SystemTray.h"

#ifdef _WIN32
    #include "../windows/SystemTray_Win32.h"
#elif __linux__
    #include "../linux/SystemTray_Linux.h"
#endif

SystemTray* SystemTray::createPlatformTray() {
    #ifdef _WIN32
        return new SystemTray_Win32();
    #elif __linux__
        return new SystemTray_Linux();
    #else
        #error "Unsupported platform"
    #endif
}