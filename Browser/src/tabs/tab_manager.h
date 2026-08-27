#ifndef SECURE_BROWSER_TAB_MANAGER_H_
#define SECURE_BROWSER_TAB_MANAGER_H_

#ifdef HAS_CEF
#include "include/cef_browser.h"
#include <map>
#include <vector>

// Stage 3: Tab Management System
// Manages the lifecycle and state of multiple child browser instances (tabs).
class TabManager {
public:
    TabManager();
    ~TabManager();

    // Create a new tab and return its internal ID.
    int CreateNewTab(CefRefPtr<CefBrowser> shell_browser, const std::string& url);

    // Switch the active visible tab.
    void SwitchToTab(int tab_id);

    // Close an existing tab securely.
    void CloseTab(int tab_id);

    // Register a successfully created browser instance.
    void RegisterBrowser(int tab_id, CefRefPtr<CefBrowser> browser);

    // Unregister upon destruction.
    void UnregisterBrowser(CefRefPtr<CefBrowser> browser);

    // Get the currently active child tab.
    CefRefPtr<CefBrowser> GetActiveTab();

private:
    // Hides a specific tab browser.
    void HideTab(int tab_id);
    // Shows a specific tab browser.
    void ShowTab(int tab_id);

    int next_tab_id_;
    int active_tab_id_;

    // Maps internal Tab ID to the actual CefBrowser instance.
    std::map<int, CefRefPtr<CefBrowser>> tabs_;
};
#else
// Mock class for Linux testing
class TabManager {
public:
    TabManager() {}
    ~TabManager() {}
};
#endif

#endif // SECURE_BROWSER_TAB_MANAGER_H_
