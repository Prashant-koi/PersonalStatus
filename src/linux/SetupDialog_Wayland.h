#ifndef SETUP_DIALOG_WAYLAND_H
#define SETUP_DIALOG_WAYLAND_H

#ifdef __linux__

#include <string>

class SetupDialog_Wayland {
public:
    static bool showSetupDialog(std::string& apiUrl, std::string& apiKey);

private:
};

#endif // __linux__
#endif // SETUP_DIALOG_WAYLAND_H