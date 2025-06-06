#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <map>
#include <iostream>

class Config {
public:
    static std::map<std::string, std::string> loadEnv(const std::string& filename = ".env") {
        std::map<std::string, std::string> env;
        std::ifstream file;
        
        // Try multiple locations for .env file
        std::vector<std::string> locations = {
            filename,           // Current directory
            "../" + filename,   // Parent directory (for build/)
            "../../" + filename // Two levels up (if needed)
        };
        
        bool fileFound = false;
        for (const auto& location : locations) {
            file.open(location);
            if (file.is_open()) {
                std::cout << "Found configuration file: " << location << std::endl;
                fileFound = true;
                break;
            }
        }
        
        if (!fileFound) {
            std::cerr << "Warning: Could not find " << filename << " in any of these locations:" << std::endl;
            for (const auto& location : locations) {
                std::cerr << "  - " << location << std::endl;
            }
            return env;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Remove whitespace around key and value
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                env[key] = value;
            }
        }
        
        file.close();
        return env;
    }
    
    static std::string getEnv(const std::map<std::string, std::string>& env, 
                             const std::string& key, 
                             const std::string& defaultValue = "") {
        auto it = env.find(key);
        return (it != env.end()) ? it->second : defaultValue;
    }
};

#endif