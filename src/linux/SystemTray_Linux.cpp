#include "SystemTray_Linux.h"
#include <iostream>

#ifdef __linux__

SystemTray_Linux::SystemTray_Linux() : menu(nullptr) {
    std::cout << "SystemTray_Linux: Constructor" << std::endl;
}

SystemTray_Linux::~SystemTray_Linux() {
    std::cout << "SystemTray_Linux: Destructor" << std::endl;
    if (menu) {
        gtk_widget_destroy(menu);  // GTK 3 API
    }
}

bool SystemTray_Linux::create() {
    std::cout << "Creating Linux system tray (GTK 3)..." << std::endl;
    
    // Create popup menu - GTK 3 API
    menu = gtk_menu_new();
    
    // Add default menu items
    addMenuItem("Show Window", 1);
    addMenuItem("Hide Window", 2);
    addSeparator();
    addMenuItem("Exit", 99);
    
    std::cout << "System tray created (basic implementation)" << std::endl;
    return true;
}

void SystemTray_Linux::show() {
    std::cout << "Showing system tray" << std::endl;
}

void SystemTray_Linux::hide() {
    std::cout << "Hiding system tray" << std::endl;
}

void SystemTray_Linux::setTooltip(const std::string& tooltip) {
    std::cout << "Setting tooltip: " << tooltip << std::endl;
}

void SystemTray_Linux::showNotification(const std::string& title, const std::string& message) {
    std::cout << "Notification: " << title << " - " << message << std::endl;
}

void SystemTray_Linux::addMenuItem(const std::string& text, int id) {
    if (!menu) return;
    
    // GTK 3 API - use gtk_menu_item_new_with_label
    GtkWidget* item = gtk_menu_item_new_with_label(text.c_str());
    g_object_set_data(G_OBJECT(item), "menu_id", GINT_TO_POINTER(id));
    g_signal_connect(item, "activate", G_CALLBACK(onMenuItemActivated), this);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);  // GTK 3 API
    gtk_widget_show(item);  // GTK 3 API
    
    menuItems.push_back(item);
    std::cout << "Added menu item: " << text << " (ID: " << id << ")" << std::endl;
}

void SystemTray_Linux::addSeparator() {
    if (!menu) return;
    
    GtkWidget* separator = gtk_separator_menu_item_new();  // GTK 3 API
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
    gtk_widget_show(separator);
    
    std::cout << "Added separator" << std::endl;
}

void SystemTray_Linux::onMenuItemActivated(GtkWidget* widget, gpointer data) {
    auto* tray = static_cast<SystemTray_Linux*>(data);
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "menu_id"));
    
    if (tray->onMenuItemClicked) {
        tray->onMenuItemClicked(id);
    }
}

void SystemTray_Linux::onTrayIconActivated(GtkWidget* widget, gpointer data) {
    auto* tray = static_cast<SystemTray_Linux*>(data);
    if (tray->onMenuItemClicked) {
        tray->onMenuItemClicked(-1); // Left click
    }
}

void SystemTray_Linux::onTrayIconPopupMenu(GtkWidget* widget, gpointer data) {
    auto* tray = static_cast<SystemTray_Linux*>(data);
    if (tray->menu) {
        gtk_widget_show_all(tray->menu);  // GTK 3 API
        gtk_menu_popup_at_pointer(GTK_MENU(tray->menu), NULL);  // GTK 3 API
    }
}

#endif // __linux__