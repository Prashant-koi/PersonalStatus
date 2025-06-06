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

bool OverlayWindow_X11 :: create() {
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
    gc = XCreateGC(display, window, 0, nullptr);
    if (font) {
        XSetFont(display, gc, font -> fid);
    }

    //setting colors
    XSetForeground(display, gc, WhitePixel(display, screen));
    XSetBackground(display, gc, BlackPixel(display, screen));

    return true;
}

void OverlayWindow_X11 :: createWindow() {
    //Get Screen dimentions
    int screenWidth = DisplayWidth(display, screen);
    int screenHeight = DisplayHeight(display, screen);

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
        BlackPixel(display, screen) //Background Color
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
                if (event.xexpose.count == 0) { //Last expose event
                    drawInterface();
                }
                break;
            
            case KeyPress:
                handleKeyPress(&event.xkey);
                break;
            
            case ButtonPress:
                handleButtonPress(&event.xbutton);
                break;

            case DestroyNotify:
                running = false;
                break;
        }
    }
}

void OverlayWindow_X11 :: handleKeyPress(XKeyEvent* event) {
    char buffer[32];
    KeySym keysym;
    int len = XLookupString(event,buffer, sizeof(buffer), &keysym, nullptr);

    if (keysym == XK_BackSpace) {
        //handle backspace
        if (!currentText.empty()) {
            currentText.pop_back();
            if (thoughtsManager) {
                thoughtsManager -> setCurrentThoughts(currentText);
            }
            drawInterface();
        }
    } else if (keysym == XK_Return) {
        // Handle enter
        drawInterface();
    } else if (len > 0 && buffer[0] >= 32) { //printable character
        currentText += buffer[0];
        if (thoughtsManager) {
            thoughtsManager -> setCurrentThoughts(currentText);
        }
        drawInterface();
    }
}

void OverlayWindow_X11 :: handleButtonPress(XButtonEvent* event) {
    //check if click is in button area
    if (event->x >= buttonArea.x &&
        event->x <= buttonArea.x + buttonArea.width &&
        event->y >= buttonArea.y &&
        event->y <= buttonArea.y + buttonArea.height) {

        // Toggle busy status
        isBusyToggled = !isBusyToggled;
        if (thoughtsManager) {
            thoughtsManager -> setBusyStatus(isBusyToggled);
        }
        drawInterface();
    }
}

void OverlayWindow_X11 :: drawInterface() {
    if (!display || !window) return;

    //Clear window
    XClearWindow(display, window);

    //Draw label
    drawText("Your current thoughts:", 10, 20);

    //Draw text input area(a rectangle)
    XDrawRectangle(display, window, gc, 
                    thoughtsArea.x, thoughtsArea.y,
                    thoughtsArea.width, thoughtsArea.height);

    //Draw current text
    if (!currentText.empty()) {
        drawText(currentText, thoughtsArea.x + 5, thoughtsArea.y + 20);
    }

    //Draw Button
    std::string buttonText = isBusyToggled ? "Busy" : "Free";
    drawButton(buttonText, buttonArea.x, buttonArea.y,
            buttonArea.width, buttonArea.height, isBusyToggled);

    XFlush(display);
}

void OverlayWindow_X11 :: drawText(const std::string& text, int x, int y) {
    XDrawString(display, window, gc, x, y, text.c_str(), text.length());
}

void OverlayWindow_X11 :: drawButton(const std::string& text, int x, int y,
                                    int w, int h, bool pressed){
    //Draw button rectangle
    if (pressed) {
        XFillRectangle(display, window, gc, x, y, w, h);
        XSetForeground(display, gc, BlackPixel(display, screen));
        drawText(text, x + 10, y + 20);
        XSetForeground(display, gc, WhitePixel(display, screen));
    } else {
        XDrawRectangle(display, window, gc, x, y, w, h);
        drawText(text, x + 10, y + 20);
    }
}

void OverlayWindow_X11 :: setThoughtsManager(ThoughtsManager* manager) {
    thoughtsManager = manager;
}

#endif //linux