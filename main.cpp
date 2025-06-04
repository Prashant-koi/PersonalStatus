#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include "AppDetector.h"
#include "WebServer.h"
#include "OverlayWindow.h"
#include "ThoughtsManager.h"

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

// Function to send JSON using Windows HTTP API
bool sendToVercelAPI(const std::string& jsonData, const std::string& apiUrl) {
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
                    // Add headers
                    LPCWSTR headers = L"Content-Type: application/json\r\n";
                    
                    BOOL headersAdded = WinHttpAddRequestHeaders(hRequest, 
                                                               headers, 
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
    std::cout << "Personal Status Monitor - Desktop Widget" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Monitoring: Brave, VS Code, PowerShell, Android Studio, Docker, Postman, Visual Studio" << std::endl;
    std::cout << "Local Server: http://localhost:8081" << std::endl;
    std::cout << "Vercel API: Pushing every 2 seconds (Windows HTTP)" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    // REPLACE THIS WITH YOUR ACTUAL VERCEL URL
    const std::string VERCEL_API_URL = "https://your-project.vercel.app/api/status";
    
    // Initialize components
    AppDetector detector;
    ThoughtsManager thoughtsManager;
    WebServer server(8081);
    OverlayWindow overlay;
    
    server.setAppDetector(&detector);
    server.setThoughtsManager(&thoughtsManager);
    
    std::cout << "Starting local web server..." << std::endl;
    
    // Start web server in background
    std::thread serverThread(&WebServer::start, &server);
    
    std::cout << "Creating overlay window..." << std::endl;
    
    // Create and show overlay window
    overlay.create();
    overlay.setThoughtsManager(&thoughtsManager);
    overlay.show();
    
    std::cout << "Starting Vercel API push loop..." << std::endl;
    
    // Start Vercel API push loop (every 2 seconds)
    std::thread vercelPushThread([&detector, &thoughtsManager, &VERCEL_API_URL]() {
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
            
            std::string jsonData = json.str();
            
            // Send to Vercel API
            if (sendToVercelAPI(jsonData, VERCEL_API_URL)) {
                std::cout << "[VERCEL] ✓ Sent: " << jsonData << std::endl;
            } else {
                std::cout << "[VERCEL] ✗ Failed to send data" << std::endl;
            }
            
            // Wait 2 seconds
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
    
    std::cout << "All components started. GUI running..." << std::endl;
    
    // Message loop for GUI (this blocks until window is closed)
    overlay.messageLoop();
    
    // Cleanup
    std::cout << "Shutting down..." << std::endl;
    server.stop();
    serverThread.join();
    vercelPushThread.detach();
    
    return 0;
}