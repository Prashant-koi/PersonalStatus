#ifdef _WIN32

#include "SetupDialog_Win32.h"
#include <commctrl.h>
#include <iostream>
#include "../resources/resources.h"

bool SetupDialog_Win32::showSetupDialog(std::string& endpoint, std::string& apiKey, bool isFirstRun) {
    DialogData data = { &endpoint, &apiKey, isFirstRun };
    
    INT_PTR result = DialogBoxParam(
        GetModuleHandle(nullptr),
        MAKEINTRESOURCE(isFirstRun ? IDD_SETUP_DIALOG : IDD_SETTINGS_DIALOG),
        nullptr,
        SetupDialogProc,
        reinterpret_cast<LPARAM>(&data)
    );
    
    return result == IDOK;
}

bool SetupDialog_Win32::showSettingsDialog(std::string& endpoint, std::string& apiKey) {
    return showSetupDialog(endpoint, apiKey, false);
}

INT_PTR CALLBACK SetupDialog_Win32::SetupDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static DialogData* dialogData = nullptr;
    
    switch (message) {
        case WM_INITDIALOG:
            {
                dialogData = reinterpret_cast<DialogData*>(lParam);
                
                // Set window title
                SetWindowText(hDlg, dialogData->isFirstRun ? 
                    L"Personal Status Monitor - First Time Setup" : 
                    L"Personal Status Monitor - Settings");
                
                // Set dialog icon (custom app icon)
                HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APP_ICON));
                if (hIcon) {
                    SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                    SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                }
                
                // Pre-fill existing values if editing
                if (!dialogData->isFirstRun) {
                    SetDlgItemTextA(hDlg, ID_ENDPOINT_EDIT, dialogData->endpoint->c_str());
                    SetDlgItemTextA(hDlg, ID_APIKEY_EDIT, dialogData->apiKey->c_str());
                } else {
                    // Set placeholder text for first run
                    SetDlgItemTextA(hDlg, ID_ENDPOINT_EDIT, "https://your-portfolio.vercel.app/api/status");
                }
                
                // Focus on endpoint field
                SetFocus(GetDlgItem(hDlg, ID_ENDPOINT_EDIT));
                return FALSE; // We set focus manually
            }
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_OK_BUTTON:
                case IDOK:
                    {
                        // Get values from dialog
                        char endpoint[512];
                        char apiKey[512];
                        
                        GetDlgItemTextA(hDlg, ID_ENDPOINT_EDIT, endpoint, sizeof(endpoint));
                        GetDlgItemTextA(hDlg, ID_APIKEY_EDIT, apiKey, sizeof(apiKey));
                        
                        // Validate input
                        if (strlen(endpoint) == 0 || strlen(apiKey) == 0) {
                            MessageBox(hDlg, L"Please fill in both the endpoint URL and API key.", 
                                     L"Missing Information", MB_ICONWARNING | MB_OK);
                            return TRUE;
                        }
                        
                        // Basic URL validation
                        if (strstr(endpoint, "http") != endpoint) {
                            MessageBox(hDlg, L"Please enter a valid URL starting with http:// or https://", 
                                     L"Invalid URL", MB_ICONWARNING | MB_OK);
                            return TRUE;
                        }
                        
                        // Save values
                        *dialogData->endpoint = std::string(endpoint);
                        *dialogData->apiKey = std::string(apiKey);
                        
                        EndDialog(hDlg, IDOK);
                        return TRUE;
                    }
                    
                case ID_CANCEL_BUTTON:
                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                    
                case ID_HELP_BUTTON:
                    {
                        const wchar_t* helpText = 
                            L"Setup Instructions:\n\n"
                            L"1. Endpoint URL: Your portfolio website API endpoint\n"
                            L"   Example: https://your-site.vercel.app/api/status\n\n"
                            L"2. API Key: A secret key to secure your endpoint\n"
                            L"   Example: psk_your_random_secure_key_here\n\n"
                            L"Need help? Check the README.md file for detailed setup instructions.";
                        
                        MessageBox(hDlg, helpText, L"Setup Help", MB_ICONINFORMATION | MB_OK);
                        return TRUE;
                    }
            }
            break;
            
        case WM_CLOSE:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
    }
    
    return FALSE;
}

#endif // _WIN32