#include "browser_client.h"
#include "../tabs/tab_manager.h"
#include "../navigation/navigation_handler.h"
#include "../downloads/download_handler.h"
#include "../history/history_manager.h"
#include "../bookmarks/bookmark_manager.h"
#include "../settings/settings_manager.h"
#include <iostream>

#ifdef HAS_CEF
#include "include/wrapper/cef_helpers.h"

// Secure IPC Handler Implementation
bool SecureMessageRouterHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         int64_t query_id,
                                         const CefString& request,
                                         bool persistent,
                                         CefRefPtr<Callback> callback) {
    CEF_REQUIRE_UI_THREAD();

    // Stage 2 Security Check: Validate Origin
    // Ensure that only our local UI can send privileged IPC messages.
    CefString url = frame->GetURL();
    std::string urlStr = url.ToString();

    // Minimal check for now: Ensure it's a local file. In a real app, strict origin validation is needed.
    if (urlStr.find("file://") != 0) {
        callback->Failure(403, "Forbidden: Untrusted origin.");
        return true;
    }

    std::string req = request.ToString();
    std::cout << "Secure IPC Received: " << req << std::endl;

    if (req == "Ping") {
        callback->Success("Pong from C++ Native!");
        return true;
    }

    // Stage 3 Tab Commands
    if (req.find("Tab_New") == 0) {
        std::string url = "https://www.google.com"; // Default new tab URL
        // In reality we need access to the TabManager here. We can cast the client.
        CefRefPtr<SecureBrowserClient> client = (SecureBrowserClient*)browser->GetHost()->GetClient().get();
        if (client && client->GetTabManager()) {
            int tab_id = client->GetTabManager()->CreateNewTab(browser, url);
            callback->Success(std::to_string(tab_id));
        } else {
            callback->Failure(500, "Tab Manager not initialized.");
        }
        return true;
    }

    if (req.find("Tab_Switch_") == 0) {
        try {
            int tab_id = std::stoi(req.substr(11));
            CefRefPtr<SecureBrowserClient> client = (SecureBrowserClient*)browser->GetHost()->GetClient().get();
            if (client && client->GetTabManager()) {
                client->GetTabManager()->SwitchToTab(tab_id);
                callback->Success("Switched");
            }
        } catch (const std::exception& e) {
            callback->Failure(400, "Invalid Tab ID");
        }
        return true;
    }

    if (req.find("Tab_Close_") == 0) {
        try {
            int tab_id = std::stoi(req.substr(10));
            CefRefPtr<SecureBrowserClient> client = (SecureBrowserClient*)browser->GetHost()->GetClient().get();
            if (client && client->GetTabManager()) {
                client->GetTabManager()->CloseTab(tab_id);
                callback->Success("Closed");
            }
        } catch (const std::exception& e) {
            callback->Failure(400, "Invalid Tab ID");
        }
        return true;
    }

    // Stage 4: Navigation Commands via IPC
    // Commands should affect the active tab, not the main shell browser.

    CefRefPtr<SecureBrowserClient> client = (SecureBrowserClient*)browser->GetHost()->GetClient().get();
    CefRefPtr<CefBrowser> active_tab = nullptr;
    if (client && client->GetTabManager()) {
        active_tab = client->GetTabManager()->GetActiveTab();
    }

    // If no active tab exists, we can't navigate.
    if (!active_tab) {
        if (req.find("Nav_") == 0) {
            callback->Failure(404, "No active tab found.");
            return true;
        }
    } else {
        if (req.find("Nav_Load_URL_") == 0) {
            std::string input_url = req.substr(13);
            NavigationHandler::LoadUrl(active_tab, input_url);
            callback->Success("Loading");
            return true;
        }

        if (req == "Nav_Back") {
            NavigationHandler::GoBack(active_tab);
            callback->Success("Back");
            return true;
        }

        if (req == "Nav_Forward") {
            NavigationHandler::GoForward(active_tab);
            callback->Success("Forward");
            return true;
        }

        if (req == "Nav_Reload") {
            NavigationHandler::Reload(active_tab);
            callback->Success("Reloaded");
            return true;
        }
    }

    // Stage 10 & 11: History and Bookmarks via IPC
    if (req.find("History_Get") == 0) {
        if (client && client->history_manager_) {
            auto history = client->history_manager_->GetRecentHistory();
            std::string json_result = "[";
            for (size_t i = 0; i < history.size(); ++i) {
                json_result += "{\"url\":\"" + history[i].url + "\", \"title\":\"" + history[i].title + "\"}";
                if (i < history.size() - 1) json_result += ",";
            }
            json_result += "]";
            callback->Success(json_result);
        } else {
            callback->Failure(500, "History manager not available");
        }
        return true;
    }

    if (req.find("History_Clear") == 0) {
        if (client && client->history_manager_) {
            client->history_manager_->ClearHistory();
            callback->Success("Cleared");
        } else {
            callback->Failure(500, "History manager not available");
        }
        return true;
    }

    if (req.find("Bookmark_Add_") == 0) {
        std::string url = req.substr(13);
        if (client && client->bookmark_manager_) {
            client->bookmark_manager_->AddBookmark(url, "Saved Bookmark");
        }
        callback->Success("Bookmark Added");
        return true;
    }

    if (req.find("Bookmark_Get") == 0) {
        if (client && client->bookmark_manager_) {
            auto bookmarks = client->bookmark_manager_->GetBookmarks();
            std::string json_result = "[";
            for (size_t i = 0; i < bookmarks.size(); ++i) {
                json_result += "{\"url\":\"" + bookmarks[i].url + "\", \"title\":\"" + bookmarks[i].title + "\"}";
                if (i < bookmarks.size() - 1) json_result += ",";
            }
            json_result += "]";
            callback->Success(json_result);
        } else {
            callback->Failure(500, "Bookmark manager not available");
        }
        return true;
    }

    if (req.find("Bookmark_Remove_") == 0) {
        std::string url = req.substr(16);
        if (client && client->bookmark_manager_) {
            client->bookmark_manager_->RemoveBookmark(url);
            callback->Success("Removed");
        } else {
            callback->Failure(500, "Bookmark manager not available");
        }
        return true;
    }

    // Stage 12: Settings
    if (req == "Settings_Open") {
        std::cout << "[Settings] Open settings requested via IPC." << std::endl;
        callback->Success("Opened");
        return true;
    }

    if (req.find("Settings_Save_") == 0) {
        // e.g. Settings_Save_en_true_true
        std::string payload = req.substr(14);

        // Parse segments split by '_'
        std::vector<std::string> parts;
        std::stringstream ss(payload);
        std::string token;
        while (std::getline(ss, token, '_')) {
            parts.push_back(token);
        }

        if (parts.size() >= 3) {
            SettingsManager::GetInstance().language = parts[0];
            SettingsManager::GetInstance().hardware_acceleration = (parts[1] == "true");
            SettingsManager::GetInstance().restore_tabs_on_startup = (parts[2] == "true");
            SettingsManager::GetInstance().SaveSettings();
            std::cout << "[Settings] Saved via UI." << std::endl;
        }

        callback->Success("Saved");
        return true;
    }

    callback->Failure(404, "Unknown command");
    return true;
}

SecureBrowserClient::SecureBrowserClient() : tab_manager_(new TabManager()),
                                             history_manager_(new HistoryManager()),
                                             bookmark_manager_(new BookmarkManager()) {
    // Initialize IPC Message Router Configuration
    CefMessageRouterConfig config;
    config.js_query_function = "cefQuery";
    config.js_cancel_function = "cefQueryCancel";

    message_router_ = CefMessageRouterBrowserSide::Create(config);
    message_handler_ = new SecureMessageRouterHandler();
    message_router_->AddHandler(message_handler_.get(), false);

    // Initialize Secure Download Handler
    download_handler_ = new SecureDownloadHandler();
}

CefRefPtr<CefDownloadHandler> SecureBrowserClient::GetDownloadHandler() {
    return download_handler_;
}

SecureBrowserClient::~SecureBrowserClient() {
    if (message_router_ && message_handler_) {
        message_router_->RemoveHandler(message_handler_.get());
    }
    if (tab_manager_) {
        delete tab_manager_;
        tab_manager_ = nullptr;
    }
    if (history_manager_) {
        delete history_manager_;
        history_manager_ = nullptr;
    }
    if (bookmark_manager_) {
        delete bookmark_manager_;
        bookmark_manager_ = nullptr;
    }
}

bool SecureBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                   CefRefPtr<CefFrame> frame,
                                                   CefProcessId source_process,
                                                   CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();
    // Route messages to the MessageRouter
    return message_router_->OnProcessMessageReceived(browser, frame, source_process, message);
}

void SecureBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    browser_list_.push_back(browser);
    std::cout << "Browser created securely. ID: " << browser->GetIdentifier() << std::endl;

    if (tab_manager_) {
        // If it's the first browser, it's the main UI shell.
        if (browser_list_.size() == 1) {
            // Shell initialized. Check if we need to restore tabs.
            if (SettingsManager::GetInstance().restore_tabs_on_startup && history_manager_) {
                auto recent = history_manager_->GetRecentHistory(3); // Restore last 3 tabs
                for (auto it = recent.rbegin(); it != recent.rend(); ++it) {
                    tab_manager_->CreateNewTab(browser, it->url);
                }
            }
        } else if (browser_list_.size() > 1) {
            // It's a child tab.
            tab_manager_->RegisterBrowser(browser->GetIdentifier(), browser);
        }
    }
}

bool SecureBrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    return false;
}

void SecureBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    browser_list_.remove(browser);

    if (browser_list_.empty()) {
        CefQuitMessageLoop();
    }
}
#endif
