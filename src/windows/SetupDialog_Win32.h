#ifndef SETUP_DIALOG_WIN32_H
#define SETUP_DIALOG_WIN32_H

#ifdef _WIN32

#include <windows.h>
#include <string>

class SetupDialog_Win32 {
public:
    static bool showSetupDialog(std::string& endpoint, std::string& apiKey, bool isFirstRun = true);
    static bool showSettingsDialog(std::string& endpoint, std::string& apiKey);
    
private:
    static INT_PTR CALLBACK SetupDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    
    struct DialogData {
        std::string* endpoint;
        std::string* apiKey;
        bool isFirstRun;
    };
    
    // Control IDs
    static const int ID_ENDPOINT_EDIT = 1001;
    static const int ID_APIKEY_EDIT = 1002;
    static const int ID_OK_BUTTON = 1003;
    static const int ID_CANCEL_BUTTON = 1004;
    static const int ID_HELP_BUTTON = 1005;
};

#endif // _WIN32
#endif