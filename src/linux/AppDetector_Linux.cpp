#include "../common/AppDetector.h"
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string>
#include <vector>
#include <algorithm>

#ifdef __linux__

// Linux-specific process detection using /proc filesystem
bool AppDetector::isProcessRunning(const std::string& processName) const {
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
}

#endif // __linux__