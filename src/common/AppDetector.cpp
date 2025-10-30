#include "AppDetector.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#elif __linux__
#include <dirent.h>
#include <fstream>
#include <string>
#include <cctype>
#endif

AppDetector::AppDetector() {
    std::cout << "AppDetector: Constructor" << std::endl;
    
#ifdef _WIN32
    // Windows app list
    apps = {
        {"Brave Browser", "brave.exe", false},
        {"Visual Studio Code", "Code.exe", false},
        {"PowerShell", "powershell.exe", false},
        {"Android Studio", "studio64.exe", false},
        {"Docker Desktop", "Docker Desktop.exe", false},
        {"Postman", "Postman.exe", false},
        {"Visual Studio", "devenv.exe", false}
    };
#elif __linux__
    // Linux app list
    apps = {
        {"Firefox", "firefox", false},
        {"Brave Browser", "brave", false},
        {"Visual Studio Code", "code", false},
        {"Terminal", "gnome-terminal", false},
        {"Hyprland", "Hyprland", false},
        {"Kitty", "kitty", false},
        {"Dolphin", "dolphin", false}
    };
#endif
}

#ifdef _WIN32
// Helper function to convert wide string to narrow string
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
#endif

bool AppDetector::isProcessRunning(const std::string& processName) const {
#ifdef _WIN32
    // Windows implementation using CreateToolhelp32Snapshot
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return false;
    }

    do {
        std::string currentProcess = wstringToString(pe32.szExeFile);
        std::transform(currentProcess.begin(), currentProcess.end(), currentProcess.begin(), ::tolower);
        
        std::string searchProcess = processName;
        std::transform(searchProcess.begin(), searchProcess.end(), searchProcess.begin(), ::tolower);
        
        if (currentProcess == searchProcess) {
            CloseHandle(hProcessSnap);
            return true;
        }
    } while (Process32NextW(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return false;

#elif __linux__
    // Linux implementation using /proc filesystem
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) return false;
    
    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        // Check if directory name is numeric (PID)
        if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
            std::string cmdline_path = "/proc/" + std::string(entry->d_name) + "/cmdline";
            std::ifstream cmdline_file(cmdline_path);
            
            if (cmdline_file.is_open()) {
                std::string cmdline;
                std::getline(cmdline_file, cmdline, '\0'); // Read until null terminator
                
                // Extract executable name
                size_t last_slash = cmdline.find_last_of('/');
                std::string exec_name = (last_slash != std::string::npos) ? 
                    cmdline.substr(last_slash + 1) : cmdline;
                
                // Convert to lowercase for comparison
                std::string exec_lower = exec_name;
                std::string process_lower = processName;
                std::transform(exec_lower.begin(), exec_lower.end(), exec_lower.begin(), ::tolower);
                std::transform(process_lower.begin(), process_lower.end(), process_lower.begin(), ::tolower);
                
                // Check for match
                if (exec_lower.find(process_lower) != std::string::npos || 
                    process_lower.find(exec_lower) != std::string::npos) {
                    closedir(proc_dir);
                    return true;
                }
            }
        }
    }
    
    closedir(proc_dir);
    return false;
#endif
}

void AppDetector::detectRunningApps() {
    for (auto& app : apps) {
        app.running = isProcessRunning(app.processName);
    }
}

void AppDetector::printResults() const {
    std::cout << "\n=== Running Applications ===" << std::endl;
    bool foundAny = false;
    
    for (const auto& app : apps) {
        if (app.running) {
            std::cout << "✓ " << app.name << " (" << app.processName << ")" << std::endl;
            foundAny = true;
        }
    }
    
    if (!foundAny) {
        std::cout << "No monitored applications are currently running." << std::endl;
    }
    std::cout << "===========================" << std::endl;
}

std::vector<AppDetector::AppInfo> AppDetector::getRunningApps() const {
    std::vector<AppInfo> runningApps;
    for (const auto& app : apps) {
        if (app.running) {
            runningApps.push_back(app);
        }
    }
    return runningApps;
}
