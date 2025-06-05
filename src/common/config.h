#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <map>

class Config {
public:
    static std::map<std::string, std::string> loadEnv(const std::string& filename = ".env") {
        std::map<std::string, std::string> env;
        std::ifstream file(filename);
        std::string line;
        
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                env[key] = value;
            }
        }
        
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