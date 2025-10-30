#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <string>
#include <atomic>

// Forward declarations
class AppDetector;
class ThoughtsManager;

class WebServer {
public:
    WebServer(int port);
    ~WebServer();
    
    bool start();
    void stop();
    void setAppDetector(AppDetector* detector);
    void setThoughtsManager(ThoughtsManager* manager);

private:
    int port;
    std::atomic<bool> running;
    AppDetector* appDetector;
    ThoughtsManager* thoughtsManager;
    int serverSocket;
    
    void acceptConnections();
    void handleClient(int clientSocket);
    std::string processRequest(const std::string& request);
};

#endif