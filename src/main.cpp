#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include "common/AppDetector.h"
#include "common/WebServer.h"
#include "common/OverlayWindow.h"
#include "common/ThoughtsManager.h"
#include "common/config.h"
#include "common/SystemTray.h"
#include <atomic>

#pragma comment(lib, "winhttp.lib")

// Function to convert string to wide string
std::wstring stringToWstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Function to extract hostname from URL
std::wstring extractHostname(const std::string& url) {
    size_t start = url.find("://");
    if (start == std::string::npos) return L"";
    start += 3;
    
    size_t end = url.find("/", start);
    if (end == std::string::npos) end = url.length();
    
    std::string hostname = url.substr(start, end - start);
    return stringToWstring(hostname);
}

// Function to extract path from URL
std::wstring extractPath(const std::string& url) {
    size_t start = url.find("://");
    if (start == std::string::npos) return L"/";
    start += 3;
    
    size_t pathStart = url.find("/", start);
    if (pathStart == std::string::npos) return L"/";
    
    std::string path = url.substr(pathStart);
    return stringToWstring(path);
}

// Function to send JSON using Windows HTTP API with API key
bool sendToVercelAPI(const std::string& jsonData, const std::string& apiUrl, const std::string& apiKey) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    bool success = false;
    
    // Initialize WinHTTP
    hSession = WinHttpOpen(L"Personal Status Monitor/1.0",
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME,
                          WINHTTP_NO_PROXY_BYPASS, 
                          0);
    
    if (hSession) {
        // Extract hostname and path from URL
        std::wstring hostname = extractHostname(apiUrl);
        std::wstring path = extractPath(apiUrl);
        
        if (!hostname.empty()) {
            hConnect = WinHttpConnect(hSession, 
                                     hostname.c_str(), 
                                     INTERNET_DEFAULT_HTTPS_PORT, 
                                     0);
            
            if (hConnect) {
                hRequest = WinHttpOpenRequest(hConnect, 
                                            L"POST", 
                                            path.c_str(),
                                            NULL, 
                                            WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_SECURE);
                
                if (hRequest) {
                    // Create headers with API key
                    std::wstring apiKeyHeader = L"X-API-Key: " + stringToWstring(apiKey) + L"\r\n";
                    std::wstring headers = L"Content-Type: application/json\r\n" + apiKeyHeader;
                    
                    BOOL headersAdded = WinHttpAddRequestHeaders(hRequest, 
                                                               headers.c_str(), 
                                                               -1, 
                                                               WINHTTP_ADDREQ_FLAG_ADD);
                    
                    if (headersAdded) {
                        // Send request
                        BOOL requestSent = WinHttpSendRequest(hRequest,
                                                            WINHTTP_NO_ADDITIONAL_HEADERS, 
                                                            0,
                                                            (LPVOID)jsonData.c_str(), 
                                                            (DWORD)jsonData.length(),
                                                            (DWORD)jsonData.length(), 
                                                            0);
                        
                        if (requestSent) {
                            if (WinHttpReceiveResponse(hRequest, NULL)) {
                                DWORD statusCode = 0;
                                DWORD statusCodeSize = sizeof(statusCode);
                                
                                if (WinHttpQueryHeaders(hRequest,
                                                      WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                                      WINHTTP_HEADER_NAME_BY_INDEX, 
                                                      &statusCode, 
                                                      &statusCodeSize, 
                                                      WINHTTP_NO_HEADER_INDEX)) {
                                    success = (statusCode >= 200 && statusCode < 300);
                                    if (!success) {
                                        std::cout << "[VERCEL] HTTP Status: " << statusCode << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Cleanup
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    
    return success;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}



int main() {

    std::atomic<bool> shouldExit(false);

    std::cout << "Personal Status Monitor - Desktop Widget" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Monitoring: Brave, VS Code, PowerShell, Android Studio, Docker, Postman, Visual Studio" << std::endl;
    std::cout << "Local Server: http://localhost:8081" << std::endl;
    std::cout << "Vercel API: Pushing every 2 seconds (Secured with API key)" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    // Load configuration from .env file
    auto env = Config::loadEnv();
    const std::string VERCEL_API_URL = Config::getEnv(env, "VERCEL_API_URL");
    const std::string API_KEY = Config::getEnv(env, "API_KEY");
    
    // Validate configuration
    if (VERCEL_API_URL.empty() || API_KEY.empty()) {
        std::cerr << "ERROR: Missing configuration!" << std::endl;
        std::cerr << "Please copy .env.example to .env and fill in your values." << std::endl;
        std::cerr << "Required: VERCEL_API_URL and API_KEY" << std::endl;
        return 1;
    }
    
    std::cout << "Configuration loaded from .env file" << std::endl;
    std::cout << "API URL: " << VERCEL_API_URL << std::endl;
    std::cout << std::endl;
    
    // Initialize components
    AppDetector detector;
    ThoughtsManager thoughtsManager;
    WebServer server(8081);
    
    server.setAppDetector(&detector);
    server.setThoughtsManager(&thoughtsManager);
    
    std::cout << "Starting local web server..." << std::endl;
    
    // Start web server in background
    std::thread serverThread(&WebServer::start, &server);
    
    // Create system tray
    std::unique_ptr<SystemTray> tray(SystemTray::createPlatformTray());
    
    // Setup tray menu
    tray->addMenuItem("Show Window", 1);
    tray->addMenuItem("Hide Window", 2);
    tray->addSeparator();
    tray->addMenuItem("Exit", 99);
    
    // Create tray
    if (!tray->create()) {
        std::cerr << "Failed to create system tray" << std::endl;
        return 1;
    }
    
    std::cout << "Creating overlay window..." << std::endl;
    
    // Create platform-specific overlay using factory method
    std::unique_ptr<OverlayWindow> overlay(OverlayWindow::createPlatformWindow());
    
    if (!overlay->create()) {
        std::cerr << "Failed to create overlay window" << std::endl;
        return 1;
    }
    
    overlay->setThoughtsManager(&thoughtsManager);
    overlay->show();
    
    // Setup tray callbacks
    bool windowVisible = true;
    tray->onMenuItemClicked = [&](int id) {
        std::cout << "[MAIN] Menu item clicked: " << id << std::endl;
        switch(id) {
            case -1: // Left click
                if (windowVisible) {
                    std::cout << "[MAIN] Hiding window..." << std::endl;
                    overlay->hide();
                    windowVisible = false;
                    
                } else {
                    std::cout << "[MAIN] Showing window..." << std::endl;
                    overlay->show();
                    windowVisible = true;
                    
                }
                break;
                
            case 1: // Show Window
                std::cout << "[MAIN] Showing window via menu..." << std::endl;
                overlay->show();
                windowVisible = true;
                
                break;
                
            case 2: // Hide Window
                std::cout << "[MAIN] Hiding window via menu..." << std::endl;
                overlay->hide();
                windowVisible = false;
                
                break;
                
            case 99: // Exit - UPDATED GRACEFUL SHUTDOWN
                std::cout << "Initiating graceful shutdown..." << std::endl;
                
                // Step 1: Set shutdown flag to stop threads
                shouldExit = true;
                
                // Step 2: Hide and destroy window
                if (windowVisible) {
                    overlay->hide();
                }
                
                // Step 3: Remove tray icon
                tray->hide();
                
                // Step 4: Stop web server
                std::cout << "Stopping web server..." << std::endl;
                server.stop();
                
                // Step 5: Exit message loop
                std::cout << "Exiting application..." << std::endl;
                PostQuitMessage(0);
                break;
        }
    };
    
    std::cout << "Starting Vercel API push loop..." << std::endl;
    
    // Start Vercel API push loop (every 2 seconds) - UPDATED
    std::thread vercelPushThread([&detector, &thoughtsManager, &VERCEL_API_URL, &API_KEY, &shouldExit]() {
        while (!shouldExit) {  // ← Check shutdown flag
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
            
            // Generate JSON with timestamp
            std::ostringstream json;
            json << "{";
            json << "\"timestamp\":" << std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() << ",";
            json << "\"thoughts\":\"" << thoughtsManager.getCurrentThoughts() << "\",";
            json << "\"activeApps\":[";
            
            for (size_t i = 0; i < activeApps.size(); ++i) {
                json << "\"" << activeApps[i] << "\"";
                if (i < activeApps.size() - 1) json << ",";
            }
            
            json << "],";
            json << "\"busy\":" << (thoughtsManager.isBusy() ? "true" : "false");
            json << "}";
            
            // Send to Vercel API
            if (sendToVercelAPI(json.str(), VERCEL_API_URL, API_KEY)) {
                std::cout << "[VERCEL] ✓ Sent: " << json.str() << std::endl;
            } else {
                std::cout << "[VERCEL] ✗ Failed to send data (check API key)" << std::endl;
            }
            
            // Wait 2 seconds, but check for shutdown every 100ms
            for (int i = 0; i < 20 && !shouldExit; ++i) {  // ← Check shutdown during wait
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        std::cout << "[VERCEL] Thread stopped gracefully" << std::endl;  // ← Confirmation
    });
    
    std::cout << "All components started. Running in background..." << std::endl;
    std::cout << "Right-click tray icon for options." << std::endl;
    
    // Non-blocking message loop
    overlay->messageLoop();  // This blocks until PostQuitMessage(0)
    
    // GRACEFUL CLEANUP SEQUENCE
    std::cout << "Message loop ended, cleaning up..." << std::endl;
    
    // Step 1: Ensure shutdown flag is set
    shouldExit = true;
    
    // Step 2: Stop web server (if not already stopped)
    std::cout << "Stopping web server..." << std::endl;
    server.stop();
    
    // Step 3: Wait for server thread to finish
    std::cout << "Waiting for server thread..." << std::endl;
    if (serverThread.joinable()) {
        serverThread.join();
        std::cout << "Server thread joined successfully" << std::endl;
    }
    
    // Step 4: Wait for Vercel API thread to finish
    std::cout << "Waiting for Vercel API thread..." << std::endl;
    if (vercelPushThread.joinable()) {
        vercelPushThread.join();  // ← Changed from detach() to join()
        std::cout << "Vercel API thread joined successfully" << std::endl;
    }
    
    // Step 5: Final cleanup
    std::cout << "All threads stopped. Goodbye!" << std::endl;
    
    return 0;
}