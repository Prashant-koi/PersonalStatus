#include "SettingsManager.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <fstream>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#endif

const std::string SettingsManager::APP_NAME = "PersonalStatusMonitor";
const std::string SettingsManager::REG_KEY_PATH = "SOFTWARE\\PersonalStatusMonitor";
const std::string SettingsManager::CONFIG_FILE = ".personal-status-config";

bool SettingsManager::saveSettings(const std::string& endpoint, const std::string& apiKey) {
    #ifdef _WIN32
    return saveToRegistry(endpoint, apiKey);
    #else
    return saveToFile(endpoint, apiKey);
    #endif
}

bool SettingsManager::loadSettings(std::string& endpoint, std::string& apiKey) {
    #ifdef _WIN32
    return loadFromRegistry(endpoint, apiKey);
    #else
    return loadFromFile(endpoint, apiKey);
    #endif
}

bool SettingsManager::hasValidSettings() {
    std::string endpoint, apiKey;
    bool loaded = loadSettings(endpoint, apiKey);
    return loaded && !endpoint.empty() && !apiKey.empty();
}

void SettingsManager::clearSettings() {
    #ifdef _WIN32
    RegDeleteKeyA(HKEY_CURRENT_USER, REG_KEY_PATH.c_str());
    #else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    std::string configPath = std::string(home) + "/" + CONFIG_FILE;
    remove(configPath.c_str());
    #endif
}

#ifdef _WIN32
bool SettingsManager::saveToRegistry(const std::string& endpoint, const std::string& apiKey) {
    HKEY hKey;
    LONG result = RegCreateKeyExA(HKEY_CURRENT_USER, REG_KEY_PATH.c_str(), 0, NULL, 
                                  REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) {
        std::cerr << "Failed to create registry key" << std::endl;
        return false;
    }
    
    // Save endpoint
    result = RegSetValueExA(hKey, "Endpoint", 0, REG_SZ, 
                           (const BYTE*)endpoint.c_str(), endpoint.length() + 1);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }
    
    // Save API key (you might want to encrypt this)
    result = RegSetValueExA(hKey, "ApiKey", 0, REG_SZ, 
                           (const BYTE*)apiKey.c_str(), apiKey.length() + 1);
    
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool SettingsManager::loadFromRegistry(std::string& endpoint, std::string& apiKey) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, REG_KEY_PATH.c_str(), 0, KEY_READ, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    // Load endpoint
    char buffer[512];
    DWORD bufferSize = sizeof(buffer);
    result = RegQueryValueExA(hKey, "Endpoint", NULL, NULL, (LPBYTE)buffer, &bufferSize);
    if (result == ERROR_SUCCESS) {
        endpoint = std::string(buffer);
    }
    
    // Load API key
    bufferSize = sizeof(buffer);
    result = RegQueryValueExA(hKey, "ApiKey", NULL, NULL, (LPBYTE)buffer, &bufferSize);
    if (result == ERROR_SUCCESS) {
        apiKey = std::string(buffer);
    }
    
    RegCloseKey(hKey);
    return !endpoint.empty() && !apiKey.empty();
}
#else
bool SettingsManager::saveToFile(const std::string& endpoint, const std::string& apiKey) {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    
    std::string configPath = std::string(home) + "/" + CONFIG_FILE;
    std::ofstream file(configPath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to create config file: " << configPath << std::endl;
        return false;
    }
    
    file << "ENDPOINT=" << endpoint << std::endl;
    file << "API_KEY=" << apiKey << std::endl;
    file.close();
    
    // Set file permissions (user read/write only)
    chmod(configPath.c_str(), S_IRUSR | S_IWUSR);
    
    return true;
}

bool SettingsManager::loadFromFile(std::string& endpoint, std::string& apiKey) {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    
    std::string configPath = std::string(home) + "/" + CONFIG_FILE;
    std::ifstream file(configPath);
    
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "ENDPOINT") {
                endpoint = value;
            } else if (key == "API_KEY") {
                apiKey = value;
            }
        }
    }
    
    file.close();
    return !endpoint.empty() && !apiKey.empty();
}
#endif