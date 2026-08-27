#ifndef SECURE_BROWSER_SETTINGS_MANAGER_H_
#define SECURE_BROWSER_SETTINGS_MANAGER_H_

#include <string>

// Stage 12: Settings Manager
class SettingsManager {
public:
    static SettingsManager& GetInstance();

    void LoadSettings();
    void SaveSettings();

    // Browser Configuration Settings
    bool restore_tabs_on_startup;
    std::string language; // "ru" or "en"
    std::string theme; // "dark" or "light"
    bool hardware_acceleration;

    // Secure persistence paths
    std::string GetCachePath() const;

private:
    SettingsManager();
    ~SettingsManager();
};

#endif // SECURE_BROWSER_SETTINGS_MANAGER_H_
