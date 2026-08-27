#ifndef SECURE_BROWSER_EXTENSION_HANDLER_H_
#define SECURE_BROWSER_EXTENSION_HANDLER_H_

#ifdef HAS_CEF
#include "include/cef_extension_handler.h"
#include <string>

// Stage 14: Extension Handler (Minimal / Disabled by default for Security)
class SecureExtensionHandler : public CefExtensionHandler {
public:
    SecureExtensionHandler();

    void OnExtensionLoadFailed(cef_errorcode_t result) override;
    void OnExtensionLoaded(CefRefPtr<CefExtension> extension) override;
    void OnExtensionUnloaded(CefRefPtr<CefExtension> extension) override;

    // Strict isolation policy
    bool OnBeforeBackgroundBrowser(CefRefPtr<CefExtension> extension,
                                   const CefString& url,
                                   CefRefPtr<CefClient>& client,
                                   CefBrowserSettings& settings) override;

private:
    IMPLEMENT_REFCOUNTING(SecureExtensionHandler);
};
#endif

#endif // SECURE_BROWSER_EXTENSION_HANDLER_H_
