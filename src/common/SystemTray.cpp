#include "SystemTray.h"

//Platform Specific
#ifdef _WIN32
    #include "../windows/SystemTray_Win32.h"
#elif __linux__
    #include "../linux/SystemTray_Linux.h"
#endif

//Factory method to create platform specifc instance
SystemTray* SystemTray :: createPlatformTray() {
    #ifdef _WIN32
        return new SystemTray_Win32();
    #elif __linux__
        return new SystemTray_Linux();
    #else
        #error "Unsupported platform"
    #endif
}