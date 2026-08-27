#include "settings_manager.h"
#include "../crypto/secure_crypto.h"
#include <iostream>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#endif

SettingsManager::SettingsManager() {
    // Default Settings
    restore_tabs_on_startup = true;
    language = "ru";
    theme = "dark";
    hardware_acceleration = true;

    LoadSettings();
}

SettingsManager::~SettingsManager() {
    SaveSettings();
}

SettingsManager& SettingsManager::GetInstance() {
    static SettingsManager instance;
    return instance;
}

std::string SettingsManager::GetCachePath() const {
    std::string path = "cef_cache"; // Fallback
#if defined(_WIN32)
    char szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        path = std::string(szPath) + "\\SecureBrowser\\Cache";
    }
#endif
    return path;
}

void SettingsManager::LoadSettings() {
    std::string settings_file = "settings.enc";
#if defined(_WIN32)
    char szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        settings_file = std::string(szPath) + "\\SecureBrowser\\settings.enc";
    }
#endif

    std::string decrypted_json;
    if (SecureCrypto::ReadEncryptedFile(settings_file, decrypted_json)) {
        // Minimal parser for tutorial scope
        if (decrypted_json.find("\"language\":\"en\"") != std::string::npos) language = "en";
        if (decrypted_json.find("\"theme\":\"light\"") != std::string::npos) theme = "light";
        if (decrypted_json.find("\"hw_accel\":false") != std::string::npos) hardware_acceleration = false;
        if (decrypted_json.find("\"restore_tabs\":false") != std::string::npos) restore_tabs_on_startup = false;
        std::cout << "[Settings] Loaded secure settings." << std::endl;
    }
}

void SettingsManager::SaveSettings() {
    std::string settings_file = "settings.enc";
#if defined(_WIN32)
    char szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        settings_file = std::string(szPath) + "\\SecureBrowser\\settings.enc";
    }
#endif

    std::stringstream json;
    json << "{";
    json << "\"language\":\"" << language << "\",";
    json << "\"theme\":\"" << theme << "\",";
    json << "\"hw_accel\":" << (hardware_acceleration ? "true" : "false") << ",";
    json << "\"restore_tabs\":" << (restore_tabs_on_startup ? "true" : "false");
    json << "}";

    if (SecureCrypto::WriteEncryptedFile(settings_file, json.str())) {
        std::cout << "[Settings] Saved secure settings." << std::endl;
    } else {
        std::cerr << "[Settings] Error saving settings securely!" << std::endl;
    }
}
