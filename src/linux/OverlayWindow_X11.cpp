#ifdef __linux__

#include "OverlayWindow_X11.h"
#include <iostream>
#include <cstring>
#include <X11/keysym.h>

OverlayWindow_X11 :: OverlayWindow_X11()
    : display(nullptr), window(0), screen(0), gc(nullptr), font(nullptr),
    isTyping(false), cursorPos(0) {

        //initializing layout
        thoughtsArea = {10, 30, 280, 60};
        buttonArea = {10, 100, 80, 30};

    }

OverlayWindow_X11 :: ~OverlayWindow_X11() {
    if (display) {
        if (font) XFreeFont(display, font);
        if (gc) XFreeGC(display, gc);
        if (window) XDestroyWindow(display, window);
        XCloseDisplay(display);
    }
}

OverlayWindow_X11 :: create() {
    //connecting to x server
    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Cannot open X display" << std::endl;
        return false;
    }

    screen = DefaultScreen(display);

    //creatign window
    createWindow();

    //loading font
    font = XLoadQueryFont(display, "fixed");
    if (!font) {
        font = XLoadQueryFont(display, "*"); //fallback
    }

    //creating gc
    gc = XCreateGc(display, window, 0, nullptr);
    if (font) {
        xSetFont(display, gc, font -> fid);
    }

    //setting colors
    XSetForeground(display, gc, WhitePixel(display, screen));
    XSetBackground(display, gc, BlackPixel(display, screen));

    return true
}
#endif