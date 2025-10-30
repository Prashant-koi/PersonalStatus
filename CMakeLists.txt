cmake_minimum_required(VERSION 3.16)
project(PersonalStatusMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static linking for Windows distribution
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Common source files
set(COMMON_SOURCES
    src/common/AppDetector.cpp
    src/common/ThoughtsManager.cpp
    src/common/WebServer.cpp
    src/common/OverlayWindow.cpp
    src/common/SystemTray.cpp
    src/common/SettingsManager.cpp          
)

# Windows-only build
if(NOT WIN32)
    message(FATAL_ERROR "This version only supports Windows. Linux/Hyprland support coming in v0.2.0")
endif()

enable_language(RC)

set(PLATFORM_SOURCES
    src/windows/OverlayWindow_Win32.cpp
    src/windows/SystemTray_Win32.cpp
    src/windows/SetupDialog_Win32.cpp    
    src/resources/resources.rc           
)

set(PLATFORM_LIBRARIES
    winhttp ws2_32 gdi32 user32 comctl32 shell32
)

add_definitions(-DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601)

# Create executable
add_executable(personal_status_monitor
    src/main.cpp
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

target_link_libraries(personal_status_monitor ${PLATFORM_LIBRARIES})
target_include_directories(personal_status_monitor PRIVATE src)

set_target_properties(personal_status_monitor PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "PersonalStatusMonitor"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)