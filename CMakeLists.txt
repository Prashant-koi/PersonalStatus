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
    src/common/config.h
)

# Platform-specific sources
if(WIN32)
    set(PLATFORM_SOURCES
        src/windows/OverlayWindow_Win32.cpp
    )
    set(PLATFORM_LIBRARIES
        winhttp ws2_32 gdi32 user32 comctl32
    )
    add_definitions(-DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601)
    
elseif(UNIX AND NOT APPLE)  # Linux
    set(PLATFORM_SOURCES
        src/linux/OverlayWindow_X11.cpp
    )
    
    # Find X11 libraries
    find_package(X11 REQUIRED)
    set(PLATFORM_LIBRARIES ${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
    
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

# Include directories
target_include_directories(personal_status_monitor PRIVATE src)