#ifndef OVERLAY_WINDOW_WAYLAND_H
#define OVERLAY_WINDOW_WAYLAND_H

#ifdef __linux__

#include "../common/OverlayWindow.h"
#include "../common/ThoughtsManager.h"
#include "../common/AutoStart.h"
#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include <string>
#include <atomic>
#include <vector>

class AppDetector;

/**
 * Wayland/Hyprland implementation of overlay window
 * Uses GTK 3 + gtk-layer-shell for desktop overlay
 */
class OverlayWindow_Wayland : public OverlayWindow {
public:
    OverlayWindow_Wayland();
    ~OverlayWindow_Wayland() override;

    // OverlayWindow interface implementation
    bool create() override;
    void show() override;
    void hide() override;
    void messageLoop() override;
    void setThoughtsManager(ThoughtsManager* mgr) override;
    void setAppDetector(AppDetector* detector) override;

    // GTK window components (public for callback access)
    GtkWidget* window;
    GtkWidget* mainBox;
    GtkWidget* thoughtsEntry;
    GtkWidget* statusLabel;
    GtkWidget* appsLabel;
    
    // Update timer
    guint updateTimerId;
    
    // Manager reference
    ThoughtsManager* thoughtsManager;
    AppDetector* appDetector;
    
    // Window state (public for callback access)
    std::atomic<bool> isVisible;
    std::atomic<bool> shouldExit;

private:
    // GTK initialization
    bool initializeGtk();
    void setupLayerShell();
    void createWidgets();
    void applyStyles();
    
    // Update methods
    void updateDisplay();
    static gboolean onUpdateTimer(gpointer userData);
    
    // Event handlers
    static gboolean onWindowDelete(GtkWidget* widget, gpointer userData);
    static void onWindowDestroy(GtkWidget* widget, gpointer userData);
    
    // Helper methods
    std::string formatStatus(bool busy);
    std::string formatApps(const std::vector<std::string>& apps);
};

// Declare callback functions outside the class
void onStatusToggled(GtkWidget* widget, gpointer userData);
void onThoughtsChanged(GtkWidget* widget, gpointer userData);
void onAutoStartToggled(GtkWidget* widget, gpointer userData);
void onCloseClicked(GtkWidget* widget, gpointer userData);

#endif // __linux__
#endif // OVERLAY_WINDOW_WAYLAND_H