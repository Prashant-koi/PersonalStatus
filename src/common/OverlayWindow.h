// This wil have abstract interface that both Windows and Linux will use
#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <string>
#include "ThoughtsManager.h"

class AppDetector;  // Forward declaration for optional app display

class OverlayWindow {
    public:

        //virtual destructur
        virtual ~OverlayWindow() =default;

        //pure virtual functions
        virtual bool create() = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void messageLoop() = 0;
        virtual void setThoughtsManager(ThoughtsManager* manager) = 0;
        // Optional hook for showing running apps; default no-op for platforms that ignore it
        virtual void setAppDetector(AppDetector* /*detector*/) {}

        //Factory Method
        static OverlayWindow* createPlatformWindow();

protected:
    //Common data in the overlay window
    ThoughtsManager* thoughtsManager = nullptr;
    bool isBusyToggled = false;


};

#endif