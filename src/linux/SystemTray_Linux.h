#ifndef SYSTEM_TRAY_LINUX_H
#define SYSTEM_TRAY_LINUX_H

#ifdef __linux__

#include "../common/SystemTray.h"
#include <gtk/gtk.h>
#include <vector>
#include <functional>

class SystemTray_Linux : public SystemTray {
public:
    SystemTray_Linux();
    ~SystemTray_Linux() override;

    bool create() override;
    void show() override;
    void hide() override;
    void setTooltip(const std::string& tooltip) override;  // ← Added
    void showNotification(const std::string& title, const std::string& message) override;  // ← Added
    void addMenuItem(const std::string& text, int id) override;
    void addSeparator() override;

private:
    GtkWidget* menu;
    std::vector<GtkWidget*> menuItems;
    
    static void onMenuItemActivated(GtkWidget* widget, gpointer data);
    static void onTrayIconActivated(GtkWidget* widget, gpointer data);
    static void onTrayIconPopupMenu(GtkWidget* widget, gpointer data);
};

#endif // __linux__
#endif // SYSTEM_TRAY_LINUX_H