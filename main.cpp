#include <iostream>
#include <chrono>
#include <thread>
#include "AppDetector.h"
#include "WebServer.h"

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
    WebServer server(8080);
    server.setAppDetector(&detector);
    
    //starting web server in seperate thread
    std::thread serverThread(&WebServer::start, &server);
    
    //keeping main thread alive and showing status update
    while (true) {
        // Clear screen for real-time updates
        clearScreen();
        
        std::cout << "Personal Status Monitor - Live Updates" << std::endl;
        std::cout << "=======================================" << std::endl;
        std::cout << "Web Server: http://localhost:8080" << std::endl;
        std::cout << std::endl;
        
        // Detect and display current app status
        detector.detectRunningApps();
        detector.printResults();
        
        std::cout << "Server running.... (Ctrl+C to stop)" << std::endl;
        
        // Wait 5 seconds before next update
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    server.stop();
    serverThread.join();
    
    return 0;
}