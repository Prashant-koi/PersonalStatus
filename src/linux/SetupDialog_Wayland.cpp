#include "SetupDialog_Wayland.h"
#include <gtk/gtk.h>
#include <iostream>
#include <cstring>

#ifdef __linux__

struct DialogData {
    GtkWidget* apiUrlEntry;
    GtkWidget* apiKeyEntry;
    std::string* apiUrl;
    std::string* apiKey;
    bool* result;
    bool* finished;
};

static void onCancelClicked(GtkWidget* widget, gpointer data) {
    DialogData* d = static_cast<DialogData*>(data);
    *d->result = false;
    *d->finished = true;
}

static void onOkClicked(GtkWidget* widget, gpointer data) {
    DialogData* d = static_cast<DialogData*>(data);
    
    // GTK 3 API - use gtk_entry_get_text
    const char* url = gtk_entry_get_text(GTK_ENTRY(d->apiUrlEntry));
    const char* key = gtk_entry_get_text(GTK_ENTRY(d->apiKeyEntry));
    
    if (url && strlen(url) > 0 && key && strlen(key) > 0) {
        *d->apiUrl = std::string(url);
        *d->apiKey = std::string(key);
        *d->result = true;
    } else {
        *d->result = false;
    }
    *d->finished = true;
}

static void onDialogDestroy(GtkWidget* widget, gpointer data) {
    DialogData* d = static_cast<DialogData*>(data);
    *d->finished = true;
}

bool SetupDialog_Wayland::showSetupDialog(std::string& apiUrl, std::string& apiKey) {
    std::cout << "Showing Wayland setup dialog..." << std::endl;
    
    bool result = false;
    bool finished = false;
    
    // Create dialog - GTK 3 API
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Personal Status Monitor - Setup");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 300);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    
    // Create main box
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    // Use newer margin functions
    gtk_widget_set_margin_start(vbox, 20);   // ← Updated
    gtk_widget_set_margin_end(vbox, 20);     // ← Updated
    gtk_widget_set_margin_top(vbox, 20);     // ← This one is fine
    gtk_widget_set_margin_bottom(vbox, 20);  // ← This one is fine
    
    // Title
    GtkWidget* title = gtk_label_new("Personal Status Monitor Setup");
    gtk_widget_set_name(title, "title-label");  // GTK 3 - use widget name instead of CSS class
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);  // GTK 3 API
    
    // API URL
    GtkWidget* apiUrlLabel = gtk_label_new("Vercel API URL:");
    gtk_widget_set_halign(apiUrlLabel, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), apiUrlLabel, FALSE, FALSE, 0);
    
    GtkWidget* apiUrlEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(apiUrlEntry), "https://yourportfolio.vercel.app/api/status");
    gtk_box_pack_start(GTK_BOX(vbox), apiUrlEntry, FALSE, FALSE, 0);
    
    // API Key
    GtkWidget* apiKeyLabel = gtk_label_new("API Key:");
    gtk_widget_set_halign(apiKeyLabel, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), apiKeyLabel, FALSE, FALSE, 0);
    
    GtkWidget* apiKeyEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(apiKeyEntry), "your-secret-api-key");
    gtk_entry_set_visibility(GTK_ENTRY(apiKeyEntry), FALSE); // Hide password
    gtk_box_pack_start(GTK_BOX(vbox), apiKeyEntry, FALSE, FALSE, 0);
    
    // Buttons
    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(buttonBox, GTK_ALIGN_END);
    
    GtkWidget* cancelButton = gtk_button_new_with_label("Cancel");
    GtkWidget* okButton = gtk_button_new_with_label("OK");
    gtk_widget_set_name(okButton, "suggested-action");  // GTK 3 - use widget name
    
    gtk_box_pack_start(GTK_BOX(buttonBox), cancelButton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonBox), okButton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), buttonBox, FALSE, FALSE, 0);
    
    // GTK 3 API - use gtk_container_add
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    // Dialog data
    DialogData data = {
        apiUrlEntry, apiKeyEntry, &apiUrl, &apiKey, &result, &finished
    };
    
    // Connect signals
    g_signal_connect(cancelButton, "clicked", G_CALLBACK(onCancelClicked), &data);
    g_signal_connect(okButton, "clicked", G_CALLBACK(onOkClicked), &data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(onDialogDestroy), &data);
    
    // Show dialog - GTK 3 API
    gtk_widget_show_all(dialog);
    
    // Wait for dialog to finish
    while (!finished) {
        g_main_context_iteration(NULL, TRUE);
    }
    
    // GTK 3 API - use gtk_widget_destroy
    gtk_widget_destroy(dialog);
    
    std::cout << "Setup dialog result: " << (result ? "OK" : "Cancel") << std::endl;
    return result;
}

#endif // __linux__