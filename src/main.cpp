#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <string>
#include <vector>
#include <atomic>
#include <csignal>

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include "windows/SetupDialog_Win32.h"
#endif

#ifdef __linux__
#include <gtk/gtk.h>
#include <curl/curl.h>
#include "linux/SetupDialog_Wayland.h"
#endif

#include "common/AppDetector.h"
#include "common/WebServer.h"
#include "common/OverlayWindow.h"
#include "common/ThoughtsManager.h"
#include "common/config.h"
#include "common/SystemTray.h"
#include "common/SettingsManager.h"

std::atomic<bool> g_shouldExit(false);

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_shouldExit = true;
}

#ifdef __linux__
// Linux HTTP implementation using curl
struct CurlResponse {
    std::string data;
    long response_code;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, CurlResponse* response) {
    size_t total_size = size * nmemb;
    response->data.append((char*)contents, total_size);
    return total_size;
}

bool sendToVercelAPI(const std::string& jsonData, const std::string& apiUrl, const std::string& apiKey) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    
    CurlResponse response;
    struct curl_slist* headers = nullptr;
    
    // Set headers
    std::string contentType = "Content-Type: application/json";
    std::string apiKeyHeader = "X-API-Key: " + apiKey;
    
    headers = curl_slist_append(headers, contentType.c_str());
    headers = curl_slist_append(headers, apiKeyHeader.c_str());
    
    // Configure curl
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
    
    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK && response.response_code >= 200 && response.response_code < 300);
}
#endif

#ifdef _WIN32
// Windows HTTP implementation (existing code)
std::wstring stringToWstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::wstring extractHostname(const std::string& url) {
    size_t start = url.find("://");
    if (start == std::string::npos) return L"";
    start += 3;
    
    size_t end = url.find("/", start);
    if (end == std::string::npos) end = url.length();
    
    std::string hostname = url.substr(start, end - start);
    return stringToWstring(hostname);
}

std::wstring extractPath(const std::string& url) {
    size_t start = url.find("://");
    if (start == std::string::npos) return L"/";
    start += 3;
    
    size_t pathStart = url.find("/", start);
    if (pathStart == std::string::npos) return L"/";
    
    std::string path = url.substr(pathStart);
    return stringToWstring(path);
}

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
#endif

int main(int argc, char* argv[]) {
    std::cout << "Personal Status Monitor - Starting..." << std::endl;

#ifdef __linux__
    // Initialize GTK3 and curl on Linux
    std::cout << "Initializing GTK3..." << std::endl;
    if (!gtk_init_check(&argc, &argv)) {
        std::cerr << "Failed to initialize GTK3!" << std::endl;
        return 1;
    }
    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::cout << "GTK3 and curl initialized" << std::endl;
#endif

    // Setup signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "Personal Status Monitor - Desktop Widget (Hyprland/Wayland Edition)" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    std::string VERCEL_API_URL, API_KEY;
    
    // Check for existing settings
    if (!SettingsManager::hasValidSettings()) {
        std::cout << "First time setup required..." << std::endl;
        
#ifdef _WIN32
        if (!SetupDialog_Win32::showSetupDialog(VERCEL_API_URL, API_KEY, true)) {
#elif __linux__
        if (!SetupDialog_Wayland::showSetupDialog(VERCEL_API_URL, API_KEY)) {
#endif
            std::cout << "Setup cancelled. Exiting..." << std::endl;
            return 0;
        }
        
        if (SettingsManager::saveSettings(VERCEL_API_URL, API_KEY)) {
            std::cout << "Settings saved successfully!" << std::endl;
        } else {
            std::cerr << "Failed to save settings!" << std::endl;
            return 1;
        }
    } else {
        if (!SettingsManager::loadSettings(VERCEL_API_URL, API_KEY)) {
            std::cerr << "Failed to load settings!" << std::endl;
            return 1;
        }
        std::cout << "Settings loaded from system storage" << std::endl;
    }
    
    std::cout << "API URL: " << VERCEL_API_URL << std::endl;
    
    try {
        // Initialize components
        AppDetector detector;
        ThoughtsManager thoughtsManager;
        WebServer server(8081);
        
        server.setAppDetector(&detector);
        server.setThoughtsManager(&thoughtsManager);
        
        std::cout << "Starting web server..." << std::endl;
        std::thread serverThread(&WebServer::start, &server);
        
        std::cout << "Creating system tray..." << std::endl;
        std::unique_ptr<SystemTray> tray(SystemTray::createPlatformTray());
        if (!tray->create()) {
            std::cerr << "Failed to create system tray" << std::endl;
            return 1;
        }
        
        std::cout << "Creating overlay window..." << std::endl;
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
            switch(id) {
                case 1: // Show Window
                    overlay->show();
                    windowVisible = true;
                    break;
                case 2: // Hide Window
                    overlay->hide();
                    windowVisible = false;
                    break;
                case 99: // Exit
                    g_shouldExit = true;
                    break;
            }
        };
        
        // Start API push thread
        std::thread apiThread([&]() {
            while (!g_shouldExit) {
                detector.detectRunningApps();
                auto runningApps = detector.getRunningApps();
                
                std::vector<std::string> activeApps;
                for (const auto& app : runningApps) {
                    if (activeApps.size() < 2) {
                        activeApps.push_back(app.name);
                    }
                }
                
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
                
                if (sendToVercelAPI(json.str(), VERCEL_API_URL, API_KEY)) {
                    std::cout << "[API] ✓ Data sent successfully" << std::endl;
                } else {
                    std::cout << "[API] ✗ Failed to send data" << std::endl;
                }
                
                for (int i = 0; i < 20 && !g_shouldExit; ++i) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        });
        
        std::cout << "Application started successfully!" << std::endl;
        
        // Main loop
#ifdef __linux__
        while (!g_shouldExit) {
            g_main_context_iteration(NULL, TRUE);
        }
#else
        overlay->messageLoop();
#endif
        
        // Cleanup
        std::cout << "Shutting down..." << std::endl;
        g_shouldExit = true;
        
        server.stop();
        
        if (serverThread.joinable()) {
            serverThread.join();
        }
        
        if (apiThread.joinable()) {
            apiThread.join();
        }
        
#ifdef __linux__
        curl_global_cleanup();
#endif
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Personal Status Monitor - Exited cleanly" << std::endl;
    return 0;
}