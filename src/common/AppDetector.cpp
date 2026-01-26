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
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <numeric>
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
    // Linux app list - more specific process names
    apps = {
        {"Firefox Browser", "firefox", false},
        {"Brave Browser", "brave", false},
        {"Chromium", "chromium", false},
        {"Visual Studio Code", "code", false},
    };
#endif
}

#if __linux__
// Levenshtein distance for fuzzy matching
static int levenshtein(const std::string& a, const std::string& b) {
    const size_t m = a.size(), n = b.size();
    std::vector<int> prev(n + 1), cur(n + 1);
    std::iota(prev.begin(), prev.end(), 0);
    for (size_t i = 1; i <= m; ++i) {
        cur[0] = static_cast<int>(i);
        for (size_t j = 1; j <= n; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            cur[j] = std::min({ prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + cost });
        }
        std::swap(prev, cur);
    }
    return prev[n];
}

static bool fuzzyMatch(const std::string& lhs, const std::string& rhs, int maxDistance = 2) {
    std::string l = lhs, r = rhs;
    std::transform(l.begin(), l.end(), l.begin(), ::tolower);
    std::transform(r.begin(), r.end(), r.begin(), ::tolower);
    int dist = levenshtein(l, r);
    // also allow substring containment as a cheap win
    return dist <= maxDistance || l.find(r) != std::string::npos || r.find(l) != std::string::npos;
}
#endif

void AppDetector::detectRunningApps() {
    // Reset all apps to not running
    for (auto& app : apps) {
        app.running = false;
    }

#ifdef __linux__
    // Linux: Check /proc for actual running processes
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) return;

    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        // Skip non-numeric directories
        if (!isdigit(entry->d_name[0])) continue;
        
        std::string pid_str = entry->d_name;
        std::string cmdline_path = "/proc/" + pid_str + "/cmdline";
        std::string stat_path = "/proc/" + pid_str + "/stat";
        
        // Check if process is actually running (not zombie)
        std::ifstream stat_file(stat_path);
        if (stat_file.is_open()) {
            std::string line;
            std::getline(stat_file, line);
            
            // Check process state (3rd field after pid and name)
            size_t first_paren = line.find('(');
            size_t last_paren = line.rfind(')');
            if (first_paren != std::string::npos && last_paren != std::string::npos && last_paren > first_paren) {
                std::string after_name = line.substr(last_paren + 1);
                size_t space_pos = after_name.find(' ');
                if (space_pos != std::string::npos) {
                    char state = after_name[space_pos + 1];
                    // Skip zombie (Z) and dead (X) processes
                    if (state == 'Z' || state == 'X') {
                        stat_file.close();
                        continue;
                    }
                }
            }
            stat_file.close();
        }
        
        // Read command line
        std::ifstream cmdline_file(cmdline_path);
        if (cmdline_file.is_open()) {
            std::string cmdline;
            std::getline(cmdline_file, cmdline);
            cmdline_file.close();
            
            if (cmdline.empty()) continue;
            
            // Extract process name from command line
            std::string process_name;
            size_t null_pos = cmdline.find('\0');
            if (null_pos != std::string::npos) {
                process_name = cmdline.substr(0, null_pos);
            } else {
                process_name = cmdline;
            }
            
            // Get basename
            size_t slash_pos = process_name.rfind('/');
            if (slash_pos != std::string::npos) {
                process_name = process_name.substr(slash_pos + 1);
            }
            
            // Check against our app list
            for (auto& app : apps) {
                if (fuzzyMatch(process_name, app.processName)) {  // ← fuzzy compare
                    if (isActiveProcess(pid_str)) {
                        app.running = true;
                    }
                    break;
                }
            }
        }
    }
    closedir(proc_dir);
#endif
}

#ifdef __linux__
bool AppDetector::isActiveProcess(const std::string& pid) const {
    // Check if process has active window or is interactive
    std::string status_path = "/proc/" + pid + "/status";
    std::ifstream status_file(status_path);
    
    if (!status_file.is_open()) return false;
    
    std::string line;
    while (std::getline(status_file, line)) {
        // Check if it's a session leader or has controlling terminal
        if (line.find("Tgid:") == 0 || line.find("SigIgn:") == 0) {
            // Simple heuristic: if it's running and we can read it, consider it active
            return true;
        }
    }
    return true;  // Default to true if we can't determine
}
#endif

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
        if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
            std::string cmdline_path = "/proc/" + std::string(entry->d_name) + "/cmdline";
            std::ifstream cmdline_file(cmdline_path);
            
            if (cmdline_file.is_open()) {
                std::string cmdline;
                std::getline(cmdline_file, cmdline, '\0');
                
                size_t last_slash = cmdline.find_last_of('/');
                std::string exec_name = (last_slash != std::string::npos) ? 
                    cmdline.substr(last_slash + 1) : cmdline;
                
                if (fuzzyMatch(exec_name, processName)) {  // ← fuzzy compare
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
