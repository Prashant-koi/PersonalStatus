#ifdef __linux__  // Only compile on Linux

#include "SystemTray_Linux.h"
#include <iostream>
#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <libnotify/notify.h>

SystemTray_Linux::SystemTray_Linux() 
    : indicator(nullptr), menu(nullptr), isCreated(false) {
}

SystemTray_Linux::~SystemTray_Linux() {
    if (menu) {
        gtk_widget_destroy(menu);
    }
    
    if (indicator) {
        g_object_unref(indicator);
    }
    
    // Cleanup libnotify
    if (notify_is_initted()) {
        notify_uninit();
    }
}

bool SystemTray_Linux::create() {
    // Initialize GTK if not already done
    if (!initializeGTK()) {
        std::cerr << "Failed to initialize GTK" << std::endl;
        return false;
    }
    
    // Initialize libnotify for notifications
    if (!notify_init("Personal Status Monitor")) {
        std::cerr << "Failed to initialize libnotify" << std::endl;
        // Continue anyway, notifications just won't work
    }
    
    // Create app indicator
    indicator = app_indicator_new("personal-status-monitor",
                                 "applications-system",  // Icon name
                                 APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    
    if (!indicator) {
        std::cerr << "Failed to create app indicator" << std::endl;
        return false;
    }
    
    // Set indicator properties
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon(indicator, "applications-system");
    
    // Create initial menu
    createMenu();
    
    isCreated = true;
    std::cout << "System tray indicator created successfully" << std::endl;
    return true;
}

void SystemTray_Linux::show() {
    if (indicator && isCreated) {
        app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    }
}

void SystemTray_Linux::hide() {
    if (indicator && isCreated) {
        app_indicator_set_status(indicator, APP_INDICATOR_STATUS_PASSIVE);
    }
}

void SystemTray_Linux::setTooltip(const std::string& tooltip) {
    // AppIndicator doesn't directly support tooltips
    // Store for potential future use or status display
    currentTooltip = tooltip;
    
    // Could be displayed in a menu item or status
    std::cout << "Tray status: " << tooltip << std::endl;
}

void SystemTray_Linux::showNotification(const std::string& title, const std::string& message) {
    if (!notify_is_initted()) {
        std::cerr << "Libnotify not initialized, cannot show notification" << std::endl;
        return;
    }
    
    NotifyNotification* notification = notify_notification_new(
        title.c_str(),
        message.c_str(),
        "applications-system"  // Icon
    );
    
    if (notification) {
        notify_notification_set_timeout(notification, 3000); // 3 seconds
        
        GError* error = nullptr;
        if (!notify_notification_show(notification, &error)) {
            if (error) {
                std::cerr << "Failed to show notification: " << error->message << std::endl;
                g_error_free(error);
            }
        }
        
        g_object_unref(notification);
    }
}

void SystemTray_Linux::addMenuItem(const std::string& text, int id) {
    LinuxMenuItem item;
    item.text = text;
    item.id = id;
    item.isSeparator = false;
    item.widget = nullptr;
    menuItems.push_back(item);
    
    // Update menu if it exists
    if (menu) {
        updateMenu();
    }
}

void SystemTray_Linux::addSeparator() {
    LinuxMenuItem item;
    item.text = "";
    item.id = 0;
    item.isSeparator = true;
    item.widget = nullptr;
    menuItems.push_back(item);
    
    // Update menu if it exists
    if (menu) {
        updateMenu();
    }
}

bool SystemTray_Linux::initializeGTK() {
    // Check if GTK is already initialized
    if (gtk_init_check(nullptr, nullptr)) {
        return true;
    }
    
    std::cerr << "GTK initialization failed" << std::endl;
    return false;
}

void SystemTray_Linux::createMenu() {
    if (menu) {
        gtk_widget_destroy(menu);
        callbackData.clear();
    }
    
    menu = gtk_menu_new();
    
    // Add menu items
    for (auto& item : menuItems) {
        if (item.isSeparator) {
            item.widget = gtk_separator_menu_item_new();
        } else {
            item.widget = gtk_menu_item_new_with_label(item.text.c_str());
            
            // Create callback data
            auto data = std::make_unique<CallbackData>();
            data->tray = this;
            data->id = item.id;
            
            // Connect signal
            g_signal_connect(item.widget, "activate",
                           G_CALLBACK(onMenuItemActivated),
                           data.get());
            
            callbackData.push_back(std::move(data));
        }
        
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item.widget);
        gtk_widget_show(item.widget);
    }
    
    // Set menu to indicator
    if (indicator) {
        app_indicator_set_menu(indicator, GTK_MENU(menu));
    }
}

void SystemTray_Linux::updateMenu() {
    createMenu();  // Recreate menu with new items
}

void SystemTray_Linux::onMenuItemActivated(GtkWidget* widget, gpointer user_data) {
    CallbackData* data = static_cast<CallbackData*>(user_data);
    if (data && data->tray && data->tray->onMenuItemClicked) {
        data->tray->onMenuItemClicked(data->id);
    }
}

#endif // __linux__