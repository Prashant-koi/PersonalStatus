#ifndef THOUGHTS_MANAGER_H
#define THOUGHTS_MANAGER_H

#include <string>
#include <mutex>

class ThoughtsManager {
public:
    ThoughtsManager();
    
    void setCurrentThoughts(const std::string& thoughts);
    std::string getCurrentThoughts() const;

    void setBusyStatus(bool busy);
    bool isBusy() const;

private:
    std::string currentThoughts;
    bool busyStatus;
    mutable std::mutex dataMutex;
};

#endif