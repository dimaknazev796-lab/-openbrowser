#include "tab_manager.h"
#include <iostream>

#ifdef HAS_CEF
#include "include/wrapper/cef_helpers.h"

// Note: For a real Windows application, showing/hiding tabs usually involves
// interacting with HWNDs (ShowWindow, SetWindowPos). For this architecture
// and security structure, we define the logical mapping.

// Initialize next_tab_id_ to 2.
// CEF ID 1 will be the Main UI Shell.
// CEF ID 2 will be the first dynamically created child tab.
TabManager::TabManager() : next_tab_id_(2), active_tab_id_(-1) {}

TabManager::~TabManager() {
    // Close all remaining tabs upon destruction for memory safety.
    for (auto& pair : tabs_) {
        if (pair.second.get()) {
            pair.second->GetHost()->CloseBrowser(true);
        }
    }
    tabs_.clear();
}

// In a full implementation, next_tab_id_ would be mapped separately,
// but CEF assigns unique integers to browsers via GetIdentifier().
// We will return 0 to the JS UI initially, and the UI will sync when OnAfterCreated fires,
// but for Stage 3/4 simplicity, we will let CEF manage the ID.

int TabManager::CreateNewTab(CefRefPtr<CefBrowser> shell_browser, const std::string& url) {
    CEF_REQUIRE_UI_THREAD();

    std::cout << "[TabManager] Requesting new tab for URL: " << url << std::endl;

    CefWindowInfo window_info;
    CefBrowserSettings browser_settings;

    // Security flags for the child tab
    browser_settings.web_security = STATE_ENABLED;
    browser_settings.plugins = STATE_DISABLED; // No legacy plugins in tabs

#if defined(_WIN32)
    // On Windows, the child browser is spawned as a child window of the main browser shell.
    // We get the HWND of the main shell browser.
    HWND parent_hwnd = shell_browser->GetHost()->GetWindowHandle();

    // RECT is normally calculated based on the UI dimensions (e.g., underneath the toolbar).
    // For now, we use a default rectangle, this is dynamically resized via IPC in a full implementation.
    RECT rect = { 0, 84, 1024, 768 }; // Example offset for the UI shell

    // Set as child window.
    window_info.SetAsChild(parent_hwnd, rect);
#endif

    // We reuse the same SecureBrowserClient for child tabs, but in a highly complex app,
    // you might separate ShellClient and TabClient.
    CefRefPtr<CefClient> client = shell_browser->GetHost()->GetClient();

    // Create the browser synchronously or asynchronously.
    CefBrowserHost::CreateBrowser(window_info, client, url, browser_settings, nullptr, nullptr);

    // CEF creates browsers asynchronously. We return a placeholder ID.
    // Real ID (CefBrowser->GetIdentifier()) is assigned in OnAfterCreated.
    // For this mock, we predict the next ID.
    // Main UI Shell = ID 1. First Tab = ID 2. Second Tab = ID 3.
    int predicted_id = next_tab_id_++;

    // To solve the async JS desync, we store the predicted ID and force mapping later,
    // or just assume synchronous creation for this specific architecture mock.
    // (A real async implementation would map a GUID returned here to the browser in OnAfterCreated)
    return predicted_id;
}

void TabManager::RegisterBrowser(int tab_id, CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    tabs_[tab_id] = browser;
    std::cout << "[TabManager] Registered Tab ID: " << tab_id << std::endl;

    // Automatically switch to the newly created tab
    SwitchToTab(tab_id);
}

void TabManager::UnregisterBrowser(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    for (auto it = tabs_.begin(); it != tabs_.end(); ++it) {
        if (it->second->IsSame(browser)) {
            std::cout << "[TabManager] Unregistered Tab ID: " << it->first << std::endl;
            tabs_.erase(it);
            break;
        }
    }
}

void TabManager::SwitchToTab(int tab_id) {
    CEF_REQUIRE_UI_THREAD();
    if (tabs_.find(tab_id) == tabs_.end()) {
        std::cerr << "[TabManager] Security/State Warning: Attempted to switch to invalid Tab ID: " << tab_id << std::endl;
        return;
    }

    if (active_tab_id_ != -1 && active_tab_id_ != tab_id) {
        HideTab(active_tab_id_);
    }

    active_tab_id_ = tab_id;
    ShowTab(active_tab_id_);
    std::cout << "[TabManager] Switched to Tab ID: " << tab_id << std::endl;
}

void TabManager::CloseTab(int tab_id) {
    CEF_REQUIRE_UI_THREAD();
    auto it = tabs_.find(tab_id);
    if (it != tabs_.end()) {
        std::cout << "[TabManager] Closing Tab ID: " << tab_id << std::endl;
        // Trigger browser closure. UnregisterBrowser will handle map cleanup.
        it->second->GetHost()->CloseBrowser(false);
    }
}

void TabManager::HideTab(int tab_id) {
    // In Windows, call ShowWindow(hwnd, SW_HIDE) on the browser host's HWND.
    auto it = tabs_.find(tab_id);
    if (it != tabs_.end() && it->second->GetHost()->HasView()) {
        it->second->GetHost()->WasHidden(true);
    }
}

void TabManager::ShowTab(int tab_id) {
    // In Windows, call ShowWindow(hwnd, SW_SHOW) and SetWindowPos.
    auto it = tabs_.find(tab_id);
    if (it != tabs_.end() && it->second->GetHost()->HasView()) {
        it->second->GetHost()->WasHidden(false);
        // Performance optimization: Give focus immediately upon showing.
        it->second->GetHost()->SetFocus(true);
    }
}

CefRefPtr<CefBrowser> TabManager::GetActiveTab() {
    CEF_REQUIRE_UI_THREAD();
    if (active_tab_id_ != -1) {
        auto it = tabs_.find(active_tab_id_);
        if (it != tabs_.end()) {
            return it->second;
        }
    }
    return nullptr;
}
#endif
