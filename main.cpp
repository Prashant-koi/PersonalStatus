#include <iostream>
#include <chrono>
#include <thread>
#include "AppDetector.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    std::cout << "Personal Status Monitor - App Detection" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Monitoring: VS Code, Brave, Blender, Steam, Spotify, Discord" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    AppDetector detector;
    
    while (true) {
        // Clear screen for real-time updates
        clearScreen();
        
        std::cout << "Personal Status Monitor - Live Updates" << std::endl;
        std::cout << "=======================================" << std::endl;
        std::cout << std::endl;
        
        // Detect and display current app status
        detector.detectRunningApps();
        detector.printResults();
        
        std::cout << "Last updated: " << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
        std::cout << "Refreshing in 5 seconds..." << std::endl;
        
        // Wait 5 seconds before next update
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    
    return 0;
}