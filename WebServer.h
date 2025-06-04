#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "AppDetector.h"
#include <string>

class WebServer {
public:
    WebServer(int port = 8080);
    void start();
    void stop();
    void setAppDetector(AppDetector* detector);

private:
    int port;
    AppDetector* appDetector;
    bool running;
    std::string generateJSON() const;
    void handleRequest(int clientSocket);
};

#endif