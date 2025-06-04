#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <windows.h>
#include <string>
#include "ThoughtsManager.h"

class OverlayWindow {
public:
    OverlayWindow();
    ~OverlayWindow();
    
    bool create();
    void show();
    void hide();
    void messageLoop();
    void setThoughtsManager(ThoughtsManager* manager);
    
private:
    HWND hwnd;
    HWND hEditThoughts;
    HWND hToggleBusy;
    HWND hLabelThoughts;
    HWND hLabelBusy;
    
    ThoughtsManager* thoughtsManager;
    bool isBusyToggled;
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void createControls();
    void updateThoughts();
    void toggleBusyStatus();
    void setupWindowStyle();
};

#endif