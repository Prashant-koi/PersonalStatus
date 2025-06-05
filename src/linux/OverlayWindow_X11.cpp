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

void OverlayWindow_X11 :: createWindow() {
    //Get Screen dimentions
    int screenWidth = DisplayWidth(display, screen);
    int screenHeight = DisplayWidth(display, screen);

    //Default window position will be at bottom right
    int windowWidth = 300;
    int windowHeight = 150;
    int xPos = screenWidth - windowWidth - 20;
    int yPos = screenHeight - windowHeight - 50;

    //Now that the positions is set we will create the widow
    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        xPos, yPos,
        windowWidth, windowHeight,
        2, //this is the border width
        WhitePixel(display, screen), //Border Color
        BlackPixel(display, screen), //Background Color
    );

    //window propertes
    XStoreName(display, window, "Set Personal Status");

    //events we are wanna receieve
    XSelectInput(display, window,
                ExposureMask | KeyPressMask | ButtonPressMask|
                StructureNotifyMask);

    //Setting window manager hints to make it stay on top
    XSetWindowAttributes attrs;
    attrs.override_redirect = True; //Bypass window manger
    XChangeWindowAttributes(display, window, CWOverrideRedirect, &attrs);
}

void OverlayWindow_X11 :: show() {
    if (display && window) {
        XMapWindow(display, window);
        XFlush(display);
    }
}

void OverlayWindow_X11 :: hide() {
    if (display && window) {
        XUnmapWindow(display, window);
        XFlush(display);
    }
}

void OverlayWindow_X11 :: messageLoop() {
    if (!display) return;

    XEvent event;
    bool running = true;

    while (running) {
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                if (event.xexpose.count = 0) { //Last expose event
                    drawInterface();
                }
                break;
            
            case KeyPress;
                handleKeyPress(&event.xkey);
                break;
            
            case ButtonPress:
                handelButtonPress(&event.xbutton);
                break;

            case DestroyNotify;
                running = false;
                break;
        }
    }
}


#endif