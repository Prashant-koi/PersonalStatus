#include "AutoStart.h"
#include <iostream>

#ifdef __linux__
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h> 
#include <cstdlib>   
#endif

#ifdef __linux__
std::string getHomeDirectory() {
    const char* home = getenv("HOME");
    if (home) return std::string(home);
    
    struct passwd* pw = getpwuid(getuid());
    return pw ? std::string(pw->pw_dir) : "/tmp";
}

std::string getAutostartDir() {
    return getHomeDirectory() + "/.config/autostart";
}

std::string getDesktopFilePath() {
    return getAutostartDir() + "/personal-status-monitor.desktop";
}
#endif

bool AutoStart::isEnabled() {
#ifdef __linux__
    std::string desktop_file = getDesktopFilePath();
    struct stat buffer;
    return (stat(desktop_file.c_str(), &buffer) == 0);
#endif
    return false;
}

bool AutoStart::enable(const std::string& executablePath) {
#ifdef __linux__
    std::string autostart_dir = getAutostartDir();
    std::string desktop_file = getDesktopFilePath();
    
    // Create autostart directory if it doesn't exist
    mkdir(autostart_dir.c_str(), 0755);
    
    // Create .desktop file
    std::ofstream file(desktop_file);
    if (!file.is_open()) {
        std::cerr << "Failed to create autostart file: " << desktop_file << std::endl;
        return false;
    }
    
    file << "[Desktop Entry]\n";
    file << "Type=Application\n";
    file << "Name=Personal Status Monitor\n";
    file << "Comment=Desktop status widget for portfolio integration\n";
    file << "Exec=" << executablePath << " --minimized\n";  // ← Start minimized to tray
    file << "Icon=applications-system\n";
    file << "Terminal=false\n";
    file << "Hidden=false\n";
    file << "X-GNOME-Autostart-enabled=true\n";
    file << "StartupNotify=false\n";
    file << "Categories=Utility;System;\n";
    
    file.close();
    
    // Make it executable
    chmod(desktop_file.c_str(), 0755);
    
    std::cout << "✅ Auto-start enabled: " << desktop_file << std::endl;
    return true;
#endif
    return false;
}

bool AutoStart::disable() {
#ifdef __linux__
    std::string desktop_file = getDesktopFilePath();
    if (remove(desktop_file.c_str()) == 0) {
        std::cout << "✅ Auto-start disabled" << std::endl;
        return true;
    }
#endif
    return false;
}

std::string AutoStart::getExecutablePath() {
#ifdef __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        result[count] = '\0';
        return std::string(result);
    }
#endif
    return "";
}