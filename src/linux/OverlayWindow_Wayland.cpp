#include "OverlayWindow_Wayland.h"
#include "../common/AutoStart.h"
#include "../common/ThoughtsManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

OverlayWindow_Wayland::OverlayWindow_Wayland()
    : window(nullptr)
    , mainBox(nullptr)
    , thoughtsEntry(nullptr)
    , statusLabel(nullptr)
    , appsLabel(nullptr)
    , updateTimerId(0)
    , thoughtsDebounceTimerId(0)  // ← Add this
    , thoughtsManager(nullptr)
    , isVisible(false)
    , shouldExit(false)
{
    std::cout << "OverlayWindow_Wayland: Constructor called" << std::endl;
}

OverlayWindow_Wayland::~OverlayWindow_Wayland() {
    std::cout << "OverlayWindow_Wayland: Destructor called" << std::endl;
    
    if (updateTimerId > 0) {
        g_source_remove(updateTimerId);
    }
    
    if (thoughtsDebounceTimerId > 0) {  // ← Add this
        g_source_remove(thoughtsDebounceTimerId);
    }
    
    if (window) {
        gtk_widget_destroy(window);
    }
}

bool OverlayWindow_Wayland::initializeGtk() {
    std::cout << "Initializing GTK3..." << std::endl;
    
    // GTK 3 doesn't have gtk_is_initialized, just assume it's initialized
    // since we call gtk_init in main()
    std::cout << "GTK3 initialized successfully" << std::endl;
    return true;
}

void OverlayWindow_Wayland::setupLayerShell() {
    std::cout << "Setting up layer shell..." << std::endl;
    
    // Initialize layer shell
    gtk_layer_init_for_window(GTK_WINDOW(window));
    
    // Set layer shell properties
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_OVERLAY);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
    
    // Set margin from edges
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, 20);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, 20);
    
    std::cout << "Layer shell setup complete" << std::endl;
}

void OverlayWindow_Wayland::createWidgets() {
    std::cout << "Creating widgets..." << std::endl;
    
    // Main container
    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(mainBox, 15);
    gtk_widget_set_margin_end(mainBox, 15);
    gtk_widget_set_margin_top(mainBox, 15);
    gtk_widget_set_margin_bottom(mainBox, 15);
    
    // Title/Status section
    GtkWidget* titleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    // Status indicator
    statusLabel = gtk_label_new("🟢 Available");
    gtk_widget_set_halign(statusLabel, GTK_ALIGN_START);
    
    // Status toggle button
    GtkWidget* statusToggle = gtk_button_new_with_label("Toggle Status");
    g_signal_connect(statusToggle, "clicked", G_CALLBACK(onStatusToggled), this);
    
    gtk_box_pack_start(GTK_BOX(titleBox), statusLabel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(titleBox), statusToggle, FALSE, FALSE, 0);
    
    // Thoughts section
    GtkWidget* thoughtsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkWidget* thoughtsTitle = gtk_label_new("💭 Your Thoughts (type to update):");
    gtk_widget_set_halign(thoughtsTitle, GTK_ALIGN_START);
    
    // Thoughts entry with REAL-TIME updates
    thoughtsEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(thoughtsEntry), "What are you working on?");
    
    // Connect to REAL-TIME text changes (no Enter key needed!)
    g_signal_connect(thoughtsEntry, "changed", G_CALLBACK(onThoughtsChanged), this);
    
    // Also keep Enter key support for users who expect it
    g_signal_connect(thoughtsEntry, "activate", G_CALLBACK(onThoughtsChanged), this);
    
    gtk_box_pack_start(GTK_BOX(thoughtsBox), thoughtsTitle, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(thoughtsBox), thoughtsEntry, FALSE, FALSE, 0);
    
    // Apps section
    appsLabel = gtk_label_new("📱 Apps: Detecting...");
    gtk_label_set_line_wrap(GTK_LABEL(appsLabel), TRUE);
    gtk_widget_set_halign(appsLabel, GTK_ALIGN_START);
    
    // Auto-start section
    GtkWidget* autostartBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* autostartLabel = gtk_label_new("🚀 Auto-start:");
    GtkWidget* autostartToggle = gtk_button_new_with_label("Enable");
    
    // Check current auto-start status
    if (AutoStart::isEnabled()) {
        gtk_button_set_label(GTK_BUTTON(autostartToggle), "Disable");
    }
    
    g_signal_connect(autostartToggle, "clicked", G_CALLBACK(onAutoStartToggled), this);
    
    gtk_box_pack_start(GTK_BOX(autostartBox), autostartLabel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(autostartBox), autostartToggle, FALSE, FALSE, 0);
    
    // Pack everything
    gtk_box_pack_start(GTK_BOX(mainBox), titleBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), thoughtsBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), appsLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), autostartBox, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(window), mainBox);
    
    std::cout << "Widgets created successfully" << std::endl;
}

void OverlayWindow_Wayland::applyStyles() {
    std::cout << "Applying CSS styles..." << std::endl;
    
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    
    const char* css = 
        "window {"
        "    background-color: rgba(30, 30, 30, 0.9);"
        "    border-radius: 10px;"
        "    border: 2px solid rgba(100, 100, 100, 0.5);"
        "}"
        "label {"
        "    color: white;"
        "    font-family: 'DejaVu Sans', sans-serif;"
        "    font-size: 12pt;"
        "}"
        "entry {"
        "    background-color: rgba(50, 50, 50, 0.8);"
        "    color: white;"
        "    border: 1px solid rgba(100, 100, 100, 0.5);"
        "    border-radius: 4px;"
        "    padding: 5px;"
        "}"
        "button {"
        "    background: linear-gradient(135deg, rgba(70, 70, 70, 0.9), rgba(50, 50, 50, 0.9));"
        "    color: white;"
        "    border: 1px solid rgba(100, 100, 100, 0.6);"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "}"
        "button:hover {"
        "    background: linear-gradient(135deg, rgba(90, 90, 90, 0.9), rgba(70, 70, 70, 0.9));"
        "    border-color: rgba(120, 120, 120, 0.8);"
        "}"
        "button:active {"
        "    background: linear-gradient(135deg, rgba(40, 40, 40, 0.9), rgba(30, 30, 30, 0.9));"
        "}"
        "box {"
        "    background-color: transparent;"
        "}";
    
    // GTK 3 API
    gtk_css_provider_load_from_data(cssProvider, css, -1, NULL);
    
    GdkScreen* screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    g_object_unref(cssProvider);
    std::cout << "CSS styles applied" << std::endl;
}

bool OverlayWindow_Wayland::create() {
    std::cout << "Creating overlay window..." << std::endl;
    
    if (!initializeGtk()) {
        return false;
    }
    
    // Create window - GTK 3 API
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Personal Status Monitor");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 150);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    // Setup layer shell
    setupLayerShell();
    
    // Create widgets
    createWidgets();
    
    // Apply styles
    applyStyles();
    
    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(onWindowDestroy), this);
    g_signal_connect(window, "delete-event", G_CALLBACK(onWindowDelete), this);
    
    std::cout << "Overlay window created successfully" << std::endl;
    return true;
}

void OverlayWindow_Wayland::show() {
    if (window) {
        gtk_widget_show_all(window);  // GTK 3 API
        isVisible = true;
        
        // Start update timer
        updateTimerId = g_timeout_add(1000, onUpdateTimer, this);
        std::cout << "Overlay window shown" << std::endl;
    }
}

void OverlayWindow_Wayland::hide() {
    if (window) {
        gtk_widget_hide(window);
        isVisible = false;
        
        if (updateTimerId > 0) {
            g_source_remove(updateTimerId);
            updateTimerId = 0;
        }
        std::cout << "Overlay window hidden" << std::endl;
    }
}

void OverlayWindow_Wayland::messageLoop() {
    std::cout << "Entering GTK main loop..." << std::endl;
    
    // GTK 3 main loop
    while (!shouldExit && gtk_main_level() >= 0) {
        gtk_main_iteration_do(FALSE);
    }
    
    std::cout << "Exited GTK main loop" << std::endl;
}

void OverlayWindow_Wayland::setThoughtsManager(ThoughtsManager* mgr) {
    thoughtsManager = mgr;
    std::cout << "ThoughtsManager set" << std::endl;
}

void OverlayWindow_Wayland::updateDisplay() {
    if (!thoughtsManager || !window) return;
    
    // Update apps only (thoughts are handled by the entry field)
    std::vector<std::string> apps;  // This should be populated by AppDetector
    std::string appsText = formatApps(apps);
    gtk_label_set_text(GTK_LABEL(appsLabel), appsText.c_str());
}

gboolean OverlayWindow_Wayland::onUpdateTimer(gpointer userData) {
    auto* self = static_cast<OverlayWindow_Wayland*>(userData);
    self->updateDisplay();
    return TRUE; // Continue timer
}

gboolean OverlayWindow_Wayland::onWindowDelete(GtkWidget* widget, gpointer userData) {
    auto* self = static_cast<OverlayWindow_Wayland*>(userData);
    self->shouldExit = true;
    return FALSE; // Allow window to be destroyed
}

void OverlayWindow_Wayland::onWindowDestroy(GtkWidget* widget, gpointer userData) {
    auto* self = static_cast<OverlayWindow_Wayland*>(userData);
    self->shouldExit = true;
    self->window = nullptr;
}

std::string OverlayWindow_Wayland::formatStatus(bool busy) {
    return busy ? "🔴 Status: Busy" : "🟢 Status: Available";
}

std::string OverlayWindow_Wayland::formatApps(const std::vector<std::string>& apps) {
    if (apps.empty()) {
        return "📱 Apps: No active applications detected";
    }
    
    std::ostringstream oss;
    oss << "📱 Apps: ";
    
    for (size_t i = 0; i < apps.size() && i < 3; ++i) {
        if (i > 0) oss << ", ";
        oss << apps[i];
    }
    
    if (apps.size() > 3) {
        oss << " (+" << (apps.size() - 3) << " more)";
    }
    
    return oss.str();
}

// Remove 'static' keyword from callback functions - they're declared in header
void onStatusToggled(GtkWidget* widget, gpointer userData) {
    auto* self = static_cast<OverlayWindow_Wayland*>(userData);
    if (!self->thoughtsManager) return;
    
    bool currentBusy = self->thoughtsManager->isBusy();
    self->thoughtsManager->setBusyStatus(!currentBusy);
    
    // Update status label
    if (!currentBusy) {
        gtk_label_set_text(GTK_LABEL(self->statusLabel), "🔴 Busy");
        gtk_button_set_label(GTK_BUTTON(widget), "Set Available");
    } else {
        gtk_label_set_text(GTK_LABEL(self->statusLabel), "🟢 Available");
        gtk_button_set_label(GTK_BUTTON(widget), "Set Busy");
    }
    
    std::cout << "Status toggled to: " << (!currentBusy ? "Busy" : "Available") << std::endl;
}

// Updated callback with debouncing
void onThoughtsChanged(GtkWidget* widget, gpointer userData) {
    auto* self = static_cast<OverlayWindow_Wayland*>(userData);
    if (!self->thoughtsManager) return;
    
    // Cancel previous debounce timer
    if (self->thoughtsDebounceTimerId > 0) {
        g_source_remove(self->thoughtsDebounceTimerId);
    }
    
    // Set new debounce timer (500ms delay) - FIX: Use class scope
    self->thoughtsDebounceTimerId = g_timeout_add(500, OverlayWindow_Wayland::onThoughtsDebounced, userData);
}

gboolean OverlayWindow_Wayland::onThoughtsDebounced(gpointer userData) {
    auto* self = static_cast<OverlayWindow_Wayland*>(userData);
    if (!self->thoughtsManager || !self->thoughtsEntry) return FALSE;
    
    // Get current text from entry
    const char* text = gtk_entry_get_text(GTK_ENTRY(self->thoughtsEntry));
    std::string thoughtsText = text ? std::string(text) : "";
    
    // Update thoughts manager
    self->thoughtsManager->setCurrentThoughts(thoughtsText);
    
    std::cout << "[THOUGHTS] Debounced update: '" << thoughtsText << "'" << std::endl;
    
    // Reset timer ID
    self->thoughtsDebounceTimerId = 0;
    return FALSE; // Don't repeat
}

void onAutoStartToggled(GtkWidget* widget, gpointer userData) {  // ← Remove 'static'
    bool isEnabled = AutoStart::isEnabled();
    
    if (isEnabled) {
        if (AutoStart::disable()) {
            gtk_button_set_label(GTK_BUTTON(widget), "Enable");
            std::cout << "Auto-start disabled" << std::endl;
        }
    } else {
        std::string execPath = AutoStart::getExecutablePath();
        if (AutoStart::enable(execPath)) {
            gtk_button_set_label(GTK_BUTTON(widget), "Disable");
            std::cout << "Auto-start enabled" << std::endl;
        }
    }
}