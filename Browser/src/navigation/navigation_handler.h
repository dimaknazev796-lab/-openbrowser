#ifndef SECURE_BROWSER_NAVIGATION_HANDLER_H_
#define SECURE_BROWSER_NAVIGATION_HANDLER_H_

#include <string>

#ifdef HAS_CEF
#include "include/cef_browser.h"

// Stage 4: Navigation and Security
class NavigationHandler {
public:
    static bool IsSecureUrl(const std::string& url);
    static std::string ParseOmniboxInput(const std::string& input);

    static void LoadUrl(CefRefPtr<CefBrowser> browser, const std::string& url);
    static void GoBack(CefRefPtr<CefBrowser> browser);
    static void GoForward(CefRefPtr<CefBrowser> browser);
    static void Reload(CefRefPtr<CefBrowser> browser, bool ignore_cache = false);
};
#else
// Mock class
class NavigationHandler {
public:
    static bool IsSecureUrl(const std::string& url);
    static std::string ParseOmniboxInput(const std::string& input);
};
#endif

#endif // SECURE_BROWSER_NAVIGATION_HANDLER_H_
