#include "OverlayWindow_Wayland.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

OverlayWindow_Wayland::OverlayWindow_Wayland()
    : window(nullptr)
    , mainBox(nullptr)
    , thoughtsLabel(nullptr)
    , statusLabel(nullptr)
    , appsLabel(nullptr)
    , updateTimerId(0)
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
    
    if (window) {
        gtk_widget_destroy(window);  // GTK 3 API
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
    
    // GTK 3 API
    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_left(mainBox, 15);    // GTK 3 API
    gtk_widget_set_margin_right(mainBox, 15);   // GTK 3 API
    gtk_widget_set_margin_top(mainBox, 15);     // GTK 3 API
    gtk_widget_set_margin_bottom(mainBox, 15);  // GTK 3 API
    
    thoughtsLabel = gtk_label_new("Thoughts: Loading...");
    gtk_label_set_line_wrap(GTK_LABEL(thoughtsLabel), TRUE);  // GTK 3 API
    gtk_label_set_max_width_chars(GTK_LABEL(thoughtsLabel), 40);
    gtk_widget_set_halign(thoughtsLabel, GTK_ALIGN_START);
    
    statusLabel = gtk_label_new("Status: Unknown");
    gtk_widget_set_halign(statusLabel, GTK_ALIGN_START);
    
    appsLabel = gtk_label_new("Apps: None");
    gtk_label_set_line_wrap(GTK_LABEL(appsLabel), TRUE);
    gtk_widget_set_halign(appsLabel, GTK_ALIGN_START);
    
    // GTK 3 API - use gtk_box_pack_start
    gtk_box_pack_start(GTK_BOX(mainBox), thoughtsLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), statusLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), appsLabel, FALSE, FALSE, 0);
    
    // GTK 3 API - use gtk_container_add
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
    
    // Update thoughts
    std::string thoughts = formatThoughts(thoughtsManager->getCurrentThoughts());
    gtk_label_set_text(GTK_LABEL(thoughtsLabel), thoughts.c_str());
    
    // Update status
    std::string status = formatStatus(thoughtsManager->isBusy());
    gtk_label_set_text(GTK_LABEL(statusLabel), status.c_str());
    
    // Update apps (placeholder)
    std::vector<std::string> apps;
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

std::string OverlayWindow_Wayland::formatThoughts(const std::string& thoughts) {
    return "💭 " + (thoughts.empty() ? "No current thoughts" : thoughts);
}

std::string OverlayWindow_Wayland::formatStatus(bool busy) {
    return busy ? "🔴 Status: Busy" : "🟢 Status: Available";
}

std::string OverlayWindow_Wayland::formatApps(const std::vector<std::string>& apps) {
    if (apps.empty()) {
        return "📱 Apps: No active applications";
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