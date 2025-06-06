#include "WebServer.h"
#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

WebServer::WebServer(int port) : port(port), appDetector(nullptr), thoughtsManager(nullptr), running(false) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverSocket = INVALID_SOCKET;
}

//Destructor
WebServer :: ~WebServer() {
    stop();
}

void WebServer::setAppDetector(AppDetector* detector) {
    appDetector = detector;
}

void WebServer::setThoughtsManager(ThoughtsManager* manager) {
    thoughtsManager = manager;
}

std::string WebServer::generateJSON() const {
    if (!appDetector || !thoughtsManager) return "{}";
    
    appDetector->detectRunningApps();
    auto runningApps = appDetector->getRunningApps();
    
    // Get top 2 running apps
    std::vector<std::string> activeApps;
    for (const auto& app : runningApps) {
        if (activeApps.size() < 2) {
            activeApps.push_back(app.name);
        }
    }
    
    std::ostringstream json;
    json << "{";
    json << "\"thoughts\":\"" << thoughtsManager->getCurrentThoughts() << "\",";
    json << "\"busy\":" << (thoughtsManager->isBusy() ? "true" : "false") << ",";
    json << "\"activeApps\":[";
    for (size_t i = 0; i < activeApps.size(); ++i) {
        json << "\"" << activeApps[i] << "\"";
        if (i < activeApps.size() - 1) json << ",";
    }
    json << "]}";
    
    return json.str();
}

void WebServer::handleRequest(int clientSocket) {
    std::string response = generateJSON();
    std::string httpResponse = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: " + std::to_string(response.length()) + "\r\n"
        "\r\n" + response;
    
    send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    closesocket(clientSocket);
}

void WebServer::start() {
    running = true;
    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        return;
    }
    
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
        return;
    }
    
    std::cout << "Web server started on port " << port << std::endl;
    
    while (running) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            std::thread(&WebServer::handleRequest, this, clientSocket).detach();
        } else if (running) {
            std::cerr << "Accept failed, still running..." << std::endl;
        }
    }
    
    closesocket(serverSocket);
    serverSocket = INVALID_SOCKET;
}

void WebServer::stop() {
    running = false;

    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
}