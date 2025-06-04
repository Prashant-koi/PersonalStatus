#include <iostream>
#include <chrono>
#include <thread>
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
    std::cout << "Personal Status Monitor - App Detection" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Monitoring: VS Code, Brave, Blender, Steam, Spotify, Discord" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    // Initialize components
    AppDetector detector;
    ThoughtsManager thoughtsManager;
    WebServer server(8080);
    OverlayWindow overlay;
    
    server.setAppDetector(&detector);
    server.setThoughtsManager(&thoughtsManager);
    
    // Start web server in background
    std::thread serverThread(&WebServer::start, &server);
    
    // Create and show overlay window
    overlay.create();
    overlay.setThoughtsManager(&thoughtsManager);
    overlay.show();
    
    // Message loop for GUI
    overlay.messageLoop();
    
    server.stop();
    serverThread.join();
    
    return 0;
}