#include "browser_client.h"
#include "../tabs/tab_manager.h"
#include "../navigation/navigation_handler.h"
#include "../downloads/download_handler.h"
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

    callback->Failure(404, "Unknown command");
    return true;
}

SecureBrowserClient::SecureBrowserClient() : tab_manager_(new TabManager()) {
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
        if (browser_list_.size() > 1) {
            // It's a child tab.
            // CEF assigns sequential identifiers starting from 1.
            // We use the CEF assigned identifier to map to the predicted ID returned to JS.
            // Since shell is ID 1, first tab is ID 2. Our next_tab_id_ counter started at 2.
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
