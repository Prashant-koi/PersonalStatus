#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>  // Add this for ostringstream
#include "AppDetector.h"
#include "WebServer.h"
#include "OverlayWindow.h"
#include "ThoughtsManager.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    std::cout << "Personal Status Monitor - Desktop Widget" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Monitoring: Brave, VS Code, PowerShell, Android Studio, Docker, Postman, Visual Studio" << std::endl;
    std::cout << "Web Server: http://localhost:8081" << std::endl;
    std::cout << "JSON logging every 3 seconds..." << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    // Initialize components
    AppDetector detector;
    ThoughtsManager thoughtsManager;
    WebServer server(8081);
    OverlayWindow overlay;
    
    server.setAppDetector(&detector);
    server.setThoughtsManager(&thoughtsManager);
    
    // Start web server in background
    std::thread serverThread(&WebServer::start, &server);
    
    // Create and show overlay window
    overlay.create();
    overlay.setThoughtsManager(&thoughtsManager);
    overlay.show();
    
    // Start periodic JSON logging in background
    std::thread loggingThread([&detector, &thoughtsManager]() {
        while (true) {
            // Detect running apps
            detector.detectRunningApps();
            auto runningApps = detector.getRunningApps();
            
            // Get top 2 running apps
            std::vector<std::string> activeApps;
            for (const auto& app : runningApps) {
                if (activeApps.size() < 2) {
                    activeApps.push_back(app.name);
                }
            }
            
            // Generate JSON
            std::ostringstream json;
            json << "{";
            json << "\"thoughts\":\"" << thoughtsManager.getCurrentThoughts() << "\",";
            json << "\"activeApps\":[";
            for (size_t i = 0; i < activeApps.size(); ++i) {
                json << "\"" << activeApps[i] << "\"";
                if (i < activeApps.size() - 1) json << ",";
            }
            json << "],";
            json << "\"busy\":" << (thoughtsManager.isBusy() ? "true" : "false");
            json << "}";
            
            // Log to console
            std::cout << "[" << std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() 
                << "] " << json.str() << std::endl;
            
            // Wait 3 seconds
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    
    // Message loop for GUI (this blocks until window is closed)
    overlay.messageLoop();
    
    // Cleanup
    server.stop();
    serverThread.join();
    loggingThread.detach(); // Let it finish naturally when main exits
    
    return 0;
}