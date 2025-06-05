#ifndef OVERLAY_WINDOW_WIN32_H
#define OVERLAY_WINDOW_WIN32_H

#ifdef _WIN32

#include "../common/OverlayWindow.h"
#include <windows.h>

class OverlayWindow_Win32 : public OverlayWindow {
    public:
        OverlayWindow_Win32();
        virtual ~OverlayWindow_Win32();

        //implementing the virtual functions form the base class "OverlayWindow"
        bool create() override;
        void show() override;
        void hide() override;
        void messageLoop() override;
        void setThoughtsManager(ThoughtsManager* manager) override;
        // void setThoughtsManager(ThoughtsManager* manager);
    private:
        //Window specific stuff below
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

#endif //_WIN32

#endif