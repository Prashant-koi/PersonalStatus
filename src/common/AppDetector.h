#ifndef APP_DETECTOR_H
#define APP_DETECTOR_H

#include <vector>
#include <string>

class AppDetector {
public:
    struct AppInfo {
        std::string name;
        std::string processName;
        bool isRunning;
    };

    AppDetector();
    void detectRunningApps();
    void printResults() const;
    std::vector<AppInfo> getRunningApps() const;

private:
    std::vector<AppInfo> apps;
    bool isProcessRunning(const std::string& processName) const;
};

#endif
