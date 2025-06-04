#include "ThoughtsManager.h"

ThoughtsManager::ThoughtsManager() : busyStatus(false) {
}

void ThoughtsManager::setCurrentThoughts(const std::string& thoughts) {
    std::lock_guard<std::mutex> lock(dataMutex);
    currentThoughts = thoughts;
}

std::string ThoughtsManager::getCurrentThoughts() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return currentThoughts;
}

void ThoughtsManager::setBusyStatus(bool busy) {
    std::lock_guard<std::mutex> lock(dataMutex);
    busyStatus = busy;
}

bool ThoughtsManager::isBusy() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return busyStatus;
}