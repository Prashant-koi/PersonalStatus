#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>
#include <map>

class SettingsManager {
public:
    // Core settings operations
    static bool saveSettings(const std::string& endpoint, const std::string& apiKey);
    static bool loadSettings(std::string& endpoint, std::string& apiKey);
    static bool hasValidSettings();
    static void clearSettings();
    
    // Platform-specific storage
    #ifdef _WIN32
    static bool saveToRegistry(const std::string& endpoint, const std::string& apiKey);
    static bool loadFromRegistry(std::string& endpoint, std::string& apiKey);
    #else
    static bool saveToFile(const std::string& endpoint, const std::string& apiKey);
    static bool loadFromFile(std::string& endpoint, std::string& apiKey);
    #endif
    
private:
    static const std::string APP_NAME;
    static const std::string REG_KEY_PATH;  // Windows
    static const std::string CONFIG_FILE;   // Linux
};

#endif