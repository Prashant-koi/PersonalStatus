cmake_minimum_required(VERSION 3.16)
project(PersonalStatusMonitor VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Building PersonalStatusMonitor v${PROJECT_VERSION} - Hyprland/Wayland")

# Common source files
set(COMMON_SOURCES
    src/common/AppDetector.cpp
    src/common/ThoughtsManager.cpp
    src/common/WebServer.cpp
    src/common/OverlayWindow.cpp
    src/common/SystemTray.cpp
    src/common/SettingsManager.cpp
    src/common/AutoStart.cpp  # ← Add this
)

# Platform detection
if(WIN32)
    message(WARNING "⚠️  You're on hyprland-wayland branch - Windows build is reference only!")
    message(STATUS "   Switch to 'main' branch for Windows development")
    
    enable_language(RC)
    set(PLATFORM_SOURCES
        src/windows/OverlayWindow_Win32.cpp
        src/windows/SystemTray_Win32.cpp
        src/windows/SetupDialog_Win32.cpp
        src/resources/resources.rc
    )
    set(PLATFORM_LIBRARIES winhttp ws2_32 gdi32 user32 comctl32 shell32)
    add_definitions(-DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601)

elseif(UNIX AND NOT APPLE)  # Linux - Hyprland/Wayland
    message(STATUS "Platform: Linux (Hyprland/Wayland) - Using GTK 3")
    
    find_package(PkgConfig REQUIRED)
    
    # Use GTK 3 (what you actually have working)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(GTK_LAYER_SHELL REQUIRED gtk-layer-shell-0)  # ← GTK 3 version
    pkg_check_modules(WAYLAND REQUIRED wayland-client)
    pkg_check_modules(CAIRO REQUIRED cairo)
    pkg_check_modules(PANGO REQUIRED pango pangocairo)
    pkg_check_modules(GLIB REQUIRED glib-2.0 gio-2.0)
    pkg_check_modules(LIBCURL REQUIRED libcurl)
    
    # Debug: Print found include directories
    message(STATUS "GTK3 include dirs: ${GTK3_INCLUDE_DIRS}")
    message(STATUS "GTK_LAYER_SHELL include dirs: ${GTK_LAYER_SHELL_INCLUDE_DIRS}")
    
    set(PLATFORM_SOURCES
        src/linux/OverlayWindow_Wayland.cpp
        src/linux/SystemTray_Linux.cpp
        src/linux/SetupDialog_Wayland.cpp
    )
    
    # Combine all libraries
    set(PLATFORM_LIBRARIES
        ${GTK3_LIBRARIES}
        ${GTK_LAYER_SHELL_LIBRARIES}
        ${WAYLAND_LIBRARIES}
        ${CAIRO_LIBRARIES}
        ${PANGO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${LIBCURL_LIBRARIES}
        pthread
    )
    
    # Combine all include directories
    set(ALL_INCLUDE_DIRS
        ${GTK3_INCLUDE_DIRS}
        ${GTK_LAYER_SHELL_INCLUDE_DIRS}
        ${WAYLAND_INCLUDE_DIRS}
        ${CAIRO_INCLUDE_DIRS}
        ${PANGO_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${LIBCURL_INCLUDE_DIRS}
    )
    
    # Add compile definitions
    add_definitions(-DWAYLAND_BUILD -DHYPRLAND_BUILD -DGTK3_BUILD)
    
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Create executable
add_executable(personal_status_monitor
    src/main.cpp
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

# Link libraries
target_link_libraries(personal_status_monitor ${PLATFORM_LIBRARIES})

# Set include directories
target_include_directories(personal_status_monitor PRIVATE 
    src
    ${ALL_INCLUDE_DIRS}
)

# Platform-specific properties
if(WIN32)
    set_target_properties(personal_status_monitor PROPERTIES
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "PersonalStatusMonitor"
    )
elseif(UNIX)
    set_target_properties(personal_status_monitor PROPERTIES
        OUTPUT_NAME "personal-status-monitor"
    )
endif()

message(STATUS "Build configuration complete")