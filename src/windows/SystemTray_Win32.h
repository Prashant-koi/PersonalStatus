#ifndef SYSTEM_TRAY_WIN32_H
#define SYSTEM_TRAY_WIN32_H

#ifdef _WIN32

#include "../common/SystemTray.h"
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

//menu item structure
struct MenuItem {
    std::string text;
    int id;
    bool isSeparator;
};

class SystemTray_Win32 : public SystemTray {
    public:
        SystemTray_Win32();
        virtual ~SystemTray_Win32();

        bool create() override;
        void show() override;
        void hide() override;
        void setTooltip(const std::string& tooltip) override;
        void showNotification(const std::string& title, const std::string& message) override;

        //Menu fucntionalities
        void addMenuItem(const std::string& text, int id) override;
        void addSeparator() override;

    private:
        //win specifc data
        NOTIFYICONDATA nid;
        HWND hwnd;
        HMENU hMenu;
        std::vector<MenuItem> menuItem;
        bool isCreated;

        //win secific constants
        static const UINT WM_TRAYICON = WM_USER + 1;
        static const UINT TRAY_ID = 1001;

        //win specific methods
        bool createHiddenWindow();
        void createContextMenu();
        void showContextMenu();
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        //Helper methods
        std::wstring stringToWstring(const std::string& str);


};


#endif //win32 one

#endif