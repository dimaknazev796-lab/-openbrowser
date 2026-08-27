#include "navigation_handler.h"
#include <iostream>
#include <regex>

// Compile implementation regardless of HAS_CEF for testing
#ifdef HAS_CEF
#include "include/wrapper/cef_helpers.h"
#endif

// Strict security checks for navigation
bool NavigationHandler::IsSecureUrl(const std::string& url) {
    // Stage 4 Security Check: Block dangerous schemes from direct navigation
    if (url.find("javascript:") == 0 || url.find("vbscript:") == 0) {
        std::cerr << "[Security Block] Attempted to navigate to executable script: " << url << std::endl;
        return false;
    }
    if (url.find("file://") == 0) {
        // In a real browser, file:// is allowed but highly restricted (no cross-origin).
        // For our secure shell, we might block it outright from the omnibox unless it's a known resource.
        std::cerr << "[Security Block] Blocked file:// access from omnibox." << std::endl;
        return false;
    }
    // data: URIs can be used for phishing or XSS
    if (url.find("data:") == 0) {
         std::cerr << "[Security Block] Blocked data: URI from omnibox." << std::endl;
         return false;
    }
    return true;
}

std::string NavigationHandler::ParseOmniboxInput(const std::string& input) {
    // If it has a scheme (http://, https://), trust it as a URL.
    if (input.find("http://") == 0 || input.find("https://") == 0 || input.find("chrome://") == 0) {
        return input;
    }

    // Basic heuristic: if it has no spaces and contains a dot, assume URL.
    // Also ensuring it doesn't accidentally have a dangerous scheme prefix that was typed without //
    if (input.find(" ") == std::string::npos && input.find(".") != std::string::npos) {
        return "https://" + input; // Force HTTPS by default (Security First)
    }

    // Explicitly fallback on letting dangerous schemes pass through if they were fully typed without a space
    // so IsSecureUrl can properly block them later rather than searching google for "javascript:alert(1)".
    if (input.find("javascript:") == 0 || input.find("file:") == 0 || input.find("data:") == 0) {
        return input;
    }

    // Otherwise, treat as a search query.
    // URL encode the query (naive implementation for demo, standard requires proper url-encoding)
    std::string encoded_query = input;
    // Replace spaces with +
    for (char& c : encoded_query) {
        if (c == ' ') c = '+';
    }

    return "https://www.google.com/search?q=" + encoded_query;
}

#ifdef HAS_CEF
void NavigationHandler::LoadUrl(CefRefPtr<CefBrowser> browser, const std::string& url) {
    CEF_REQUIRE_UI_THREAD();
    if (browser && browser->GetMainFrame()) {
        std::string final_url = ParseOmniboxInput(url);

        if (IsSecureUrl(final_url)) {
            std::cout << "[Navigation] Loading URL: " << final_url << std::endl;
            browser->GetMainFrame()->LoadURL(final_url);
        }
    }
}

void NavigationHandler::GoBack(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    if (browser && browser->CanGoBack()) {
        browser->GoBack();
    }
}

void NavigationHandler::GoForward(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    if (browser && browser->CanGoForward()) {
        browser->GoForward();
    }
}

void NavigationHandler::Reload(CefRefPtr<CefBrowser> browser, bool ignore_cache) {
    CEF_REQUIRE_UI_THREAD();
    if (browser) {
        if (ignore_cache) {
            browser->ReloadIgnoreCache();
        } else {
            browser->Reload();
        }
    }
}
#endif // HAS_CEF
