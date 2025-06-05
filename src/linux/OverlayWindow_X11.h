#ifndef OVERLAY_WINDOW_X11_H
#define OVERLAY_WINDOW_X11_H

#ifdef __linux__ //only for linux

#include "../common/OverlayWindow.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>

class OverlayWindow_X11 : public OverlayWindow {
public:
    OverlayWindow_X11();
    virtual ~OverlayWindow_X11();

    //implement interface check windows one forbetter definition
    bool create() override;
    void show() override;
    void hide() override;
    void messageLoop() override;
    void setThoughtManager(ThoughtsManager* manager) override;

private:
    //X11-specific stuff from now on

    //Data structure
    Display* display;
    Window window; //our wondow
    int screen; //screen
    GC gc; //graphics context
    XFontStruct* font; //Font for any texts

    //Input Handeling belwo
    std::string currentText;
    bool isTyping;
    int cursorPos;

    //UI layout belwo
    struct {
        int x, y, width, height;
    } thoughtsArea, buttonArea;

    //methods
    void createWindow();
    void handleKeyPress(XKeyEvent* event);
    void handleButtonPress(XButtonEvent* event);
    void handleExpose(XExposeEvent* event);
    void drawInterface();
    void drawText(const std::string& text, int x, int y);
    void drawButton(const std::string& text, int x, int y, int w, int h, bool pressed);

};
#endif //linux one
#endif