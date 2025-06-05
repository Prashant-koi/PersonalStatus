#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "AppDetector.h"
#include "ThoughtsManager.h"
#include <string>

class WebServer {
public:
    WebServer(int port = 8080);
    void start();
    void stop();
    void setAppDetector(AppDetector* detector);
    void setThoughtsManager(ThoughtsManager* manager);

private:
    int port;
    AppDetector* appDetector;
    ThoughtsManager* thoughtsManager;
    bool running;
    std::string generateJSON() const;
    void handleRequest(int clientSocket);
};

#endif