#include "extension_handler.h"
#include <iostream>

#ifdef HAS_CEF
#include "include/wrapper/cef_helpers.h"

SecureExtensionHandler::SecureExtensionHandler() {}

void SecureExtensionHandler::OnExtensionLoadFailed(cef_errorcode_t result) {
    std::cerr << "[Security] Extension load failed. Code: " << result << std::endl;
}

void SecureExtensionHandler::OnExtensionLoaded(CefRefPtr<CefExtension> extension) {
    std::cout << "[Security] Extension loaded securely: " << extension->GetIdentifier().ToString() << std::endl;
}

void SecureExtensionHandler::OnExtensionUnloaded(CefRefPtr<CefExtension> extension) {
    std::cout << "[Security] Extension unloaded: " << extension->GetIdentifier().ToString() << std::endl;
}

bool SecureExtensionHandler::OnBeforeBackgroundBrowser(CefRefPtr<CefExtension> extension,
                                                       const CefString& url,
                                                       CefRefPtr<CefClient>& client,
                                                       CefBrowserSettings& settings) {
    CEF_REQUIRE_UI_THREAD();

    // Security: Do not allow background pages by default to prevent silent abuse.
    std::cerr << "[Security Block] Blocked background page execution for extension: " << extension->GetIdentifier().ToString() << std::endl;

    return true; // true means cancel the background browser creation
}

#endif
