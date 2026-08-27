#ifndef SECURE_BROWSER_PERMISSION_HANDLER_H_
#define SECURE_BROWSER_PERMISSION_HANDLER_H_

#ifdef HAS_CEF
#include "include/cef_permission_handler.h"
#include <string>

// Stage 13: Secure Permissions Management
class SecurePermissionHandler : public CefPermissionHandler {
public:
    SecurePermissionHandler();

    // Invoked when a page requests a specific permission (e.g. geolocation, audio, video).
    bool OnRequestMediaAccessPermission(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& requesting_url,
        uint32_t requested_permissions,
        CefRefPtr<CefMediaAccessCallback> callback) override;

    bool OnShowPermissionPrompt(
        CefRefPtr<CefBrowser> browser,
        uint64_t prompt_id,
        const CefString& requesting_origin,
        uint32_t requested_permissions,
        CefRefPtr<CefPermissionPromptCallback> callback) override;

    void OnDismissPermissionPrompt(
        CefRefPtr<CefBrowser> browser,
        uint64_t prompt_id,
        cef_permission_request_result_t result) override;

private:
    IMPLEMENT_REFCOUNTING(SecurePermissionHandler);
};
#endif

#endif // SECURE_BROWSER_PERMISSION_HANDLER_H_
