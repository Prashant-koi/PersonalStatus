#ifndef SYSTEM_TRAY_H
#define SYSTEM_TRAY_H

#include <string>
#include <functional>

class SystemTray {
    public:
        virtual ~SystemTray() = default;

        //core tray functionality
        virtual bool create() = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void setTooltip(const std::string& tooltip) = 0;
        virtual void showNotification(const std::string& title, const std::string& message) = 0;

        //Menu
        virtual void addMenuItem(const std::string& text, int id) =0;
        virtual void addSeparator() = 0;


        //Factorry method
        static SystemTray* createPlatformTray();

        //Callback 
        std::function<void(int)> onMenuItemClicked;


};

#endif