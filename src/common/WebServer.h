#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include "AppDetector.h"
#include "ThoughtsManager.h"
#include <string>

class WebServer {
public:
    WebServer(int port = 8080);
    ~WebServer();
    void start();
    void stop();
    void setAppDetector(AppDetector* detector);
    void setThoughtsManager(ThoughtsManager* manager);

private:
    int port;
    AppDetector* appDetector;
    ThoughtsManager* thoughtsManager;
    bool running;
#ifdef _WIN32
    SOCKET serverSocket;
#endif
    std::string generateJSON() const;
    void handleRequest(int clientSocket);
};

#endif