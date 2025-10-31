#ifndef AUTOSTART_H
#define AUTOSTART_H

#include <string>

class AutoStart {
public:
    static bool isEnabled();
    static bool enable(const std::string& executablePath);
    static bool disable();
    static std::string getExecutablePath();
    static bool createApplicationEntry(const std::string& executablePath);
};

#endif