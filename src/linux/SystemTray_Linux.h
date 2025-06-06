#ifndef SYSTEM_TRAY_LINUX_H
#define SYSTEM_TRAY_LINUX_H

#ifdef __linux__  // Only compile on Linux

#include "../common/SystemTray.h"
#include <string>
#include <vector>
#include <memory>

// Forward declarations for GTK/AppIndicator
typedef struct _AppIndicator AppIndicator;
typedef struct _GtkWidget GtkWidget;

// Menu item structure
struct LinuxMenuItem {
    std::string text;
    int id;
    bool isSeparator;
    GtkWidget* widget;
};

class SystemTray_Linux : public SystemTray {
public:
    SystemTray_Linux();
    virtual ~SystemTray_Linux();
    
    // Implement abstract interface
    bool create() override;
    void show() override;
    void hide() override;
    void setTooltip(const std::string& tooltip) override;
    void showNotification(const std::string& title, const std::string& message) override;
    
    // Menu functionality
    void addMenuItem(const std::string& text, int id) override;
    void addSeparator() override;
    
private:
    // Linux-specific data
    AppIndicator* indicator;
    GtkWidget* menu;
    std::vector<LinuxMenuItem> menuItems;
    bool isCreated;
    std::string currentTooltip;
    
    // Linux-specific methods
    bool initializeGTK();
    void createMenu();
    void updateMenu();
    
    // Static callback methods (required for GTK)
    static void onMenuItemActivated(GtkWidget* widget, gpointer user_data);
    
    // Helper to store callback data
    struct CallbackData {
        SystemTray_Linux* tray;
        int id;
    };
    std::vector<std::unique_ptr<CallbackData>> callbackData;
};

#endif // __linux__
#endif