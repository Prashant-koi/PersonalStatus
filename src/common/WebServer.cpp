#include "WebServer.h"
#include "ThoughtsManager.h"
#include "AppDetector.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#elif __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

WebServer::WebServer(int port) : port(port), running(false), appDetector(nullptr), thoughtsManager(nullptr) {
    std::cout << "WebServer: Constructor, port = " << port << std::endl;
}

WebServer::~WebServer() {
    std::cout << "WebServer: Destructor" << std::endl;
    stop();
}

bool WebServer::start() {
    std::cout << "Starting web server on port " << port << std::endl;
    
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }
#endif

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    // Set socket options
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    running = true;
    std::cout << "Web server started on port " << port << std::endl;
    
    // Start accepting connections in a separate thread
    std::thread acceptThread(&WebServer::acceptConnections, this);
    acceptThread.detach();
    
    return true;
}

void WebServer::stop() {
    if (running) {
        std::cout << "Stopping web server..." << std::endl;
        running = false;
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
    }
}

void WebServer::acceptConnections() {
    while (running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == INVALID_SOCKET) {
            if (running) {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }

        // Handle client in separate thread
        std::thread clientThread(&WebServer::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void WebServer::handleClient(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string request(buffer);
        
        std::string response = processRequest(request);
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    
    closesocket(clientSocket);
}

std::string WebServer::processRequest(const std::string& request) {
    std::cout << "Processing request..." << std::endl;
    
    // Simple HTTP response
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: application/json\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n\r\n";
    
    // JSON response with current status
    response << "{\n";
    response << "  \"timestamp\": " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << ",\n";
    
    if (thoughtsManager) {
        response << "  \"thoughts\": \"" << thoughtsManager->getCurrentThoughts() << "\",\n";
        response << "  \"busy\": " << (thoughtsManager->isBusy() ? "true" : "false") << ",\n";
    }
    
    response << "  \"apps\": []"; // TODO: Get from AppDetector
    response << "\n}";
    
    return response.str();
}

void WebServer::setAppDetector(AppDetector* detector) {
    appDetector = detector;
    std::cout << "AppDetector set for WebServer" << std::endl;
}

void WebServer::setThoughtsManager(ThoughtsManager* manager) {
    thoughtsManager = manager;
    std::cout << "ThoughtsManager set for WebServer" << std::endl;
}