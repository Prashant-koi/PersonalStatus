#include "AppDetector.h"
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <algorithm>

AppDetector::AppDetector() {
    // Initialize the list of apps to monitor - Updated list
    apps = {
        {"Brave Browser", "brave.exe", false},
        {"Visual Studio Code", "Code.exe", false},
        {"PowerShell", "powershell.exe", false},
        {"Android Studio", "studio64.exe", false},
        {"Docker Desktop", "Docker Desktop.exe", false},
        {"Postman", "Postman.exe", false},
        {"Visual Studio", "devenv.exe", false}
    };
}

// Helper function to convert wide string to narrow string
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

bool AppDetector::isProcessRunning(const std::string& processName) const {
    HANDLE hProcessSnap;
    PROCESSENTRY32W pe32;  // Use PROCESSENTRY32W explicitly for wide chars
    
    // Take a snapshot of all processes in the system
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    pe32.dwSize = sizeof(PROCESSENTRY32W);  // Use wide version
    
    // Retrieve information about the first process
    if (!Process32FirstW(hProcessSnap, &pe32)) {  // Use wide version
        CloseHandle(hProcessSnap);
        return false;
    }
    
    // Walk through the snapshot of processes
    do {
        // Convert wide string to narrow string 
        std::wstring wideProcessName(pe32.szExeFile, wcslen(pe32.szExeFile));
        std::string currentProcess = wstringToString(wideProcessName);
        
        // Convert to lowercase for case-insensitive comparison
        std::transform(currentProcess.begin(), currentProcess.end(), 
                      currentProcess.begin(), ::tolower);
        
        std::string targetProcess = processName;
        std::transform(targetProcess.begin(), targetProcess.end(), 
                      targetProcess.begin(), ::tolower);
        
        if (currentProcess == targetProcess) {
            CloseHandle(hProcessSnap);
            return true;
        }
    } while (Process32NextW(hProcessSnap, &pe32));  // Use wide version
    
    CloseHandle(hProcessSnap);
    return false;
}

void AppDetector::detectRunningApps() {
    for (auto& app : apps) {
        app.isRunning = isProcessRunning(app.processName);
    }
}

void AppDetector::printResults() const {
    std::cout << "=== Current App Status ===" << std::endl;
    std::cout << std::endl;
    
    // Print running apps
    std::cout << "Currently Running:" << std::endl;
    bool hasRunningApps = false;
    for (const auto& app : apps) {
        if (app.isRunning) {
            std::cout << "  ✓ " << app.name << std::endl;
            hasRunningApps = true;
        }
    }
    if (!hasRunningApps) {
        std::cout << "  None of the monitored apps are running." << std::endl;
    }
    
    std::cout << std::endl;
    
    // Print not running apps
    std::cout << "Not Running:" << std::endl;
    bool hasStoppedApps = false;
    for (const auto& app : apps) {
        if (!app.isRunning) {
            std::cout << "  ✗ " << app.name << std::endl;
            hasStoppedApps = true;
        }
    }
    if (!hasStoppedApps) {
        std::cout << "  All monitored apps are running!" << std::endl;
    }
    
    std::cout << std::endl;
}

std::vector<AppDetector::AppInfo> AppDetector::getRunningApps() const {
    std::vector<AppInfo> runningApps;
    for (const auto& app : apps) {
        if (app.isRunning) {
            runningApps.push_back(app);
        }
    }
    return runningApps;
}
