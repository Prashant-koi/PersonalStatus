cmake_minimum_required(VERSION 3.16)
project(PersonalStatusMonitor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common source files (cross-platform)
set(COMMON_SOURCES
    src/common/AppDetector.cpp
    src/common/ThoughtsManager.cpp
    src/common/WebServer.cpp
    src/common/OverlayWindow.cpp
    src/common/SystemTray.cpp          # NEW
)

# Platform-specific sources
if(WIN32)
    set(PLATFORM_SOURCES
        src/windows/OverlayWindow_Win32.cpp
        src/windows/SystemTray_Win32.cpp    # NEW
    )
    set(PLATFORM_LIBRARIES
        winhttp ws2_32 gdi32 user32 comctl32 shell32  # Added shell32 for tray
    )
    add_definitions(-DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601)
    
elseif(UNIX AND NOT APPLE)  # Linux
    set(PLATFORM_SOURCES
        src/linux/OverlayWindow_X11.cpp
        src/linux/SystemTray_Linux.cpp     # NEW
    )
    
    # Find required libraries
    find_package(X11 REQUIRED)
    find_package(PkgConfig REQUIRED)
    
    # Check for AppIndicator and libnotify
    pkg_check_modules(APPINDICATOR REQUIRED appindicator3-0.1)
    pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    
    set(PLATFORM_LIBRARIES 
        ${X11_LIBRARIES} 
        ${APPINDICATOR_LIBRARIES}
        ${LIBNOTIFY_LIBRARIES}
        ${GTK3_LIBRARIES}
    )
    
    include_directories(
        ${X11_INCLUDE_DIR}
        ${APPINDICATOR_INCLUDE_DIRS}
        ${LIBNOTIFY_INCLUDE_DIRS}
        ${GTK3_INCLUDE_DIRS}
    )
    
endif()

# Create executable
add_executable(personal_status_monitor
    src/main.cpp
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

# Link libraries
target_link_libraries(personal_status_monitor ${PLATFORM_LIBRARIES})

# Include directories
target_include_directories(personal_status_monitor PRIVATE src)