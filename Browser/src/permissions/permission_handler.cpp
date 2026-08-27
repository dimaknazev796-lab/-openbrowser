#include "permission_handler.h"
#include <iostream>

#ifdef HAS_CEF
#include "include/wrapper/cef_helpers.h"

SecurePermissionHandler::SecurePermissionHandler() {}

bool SecurePermissionHandler::OnRequestMediaAccessPermission(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& requesting_url,
    uint32_t requested_permissions,
    CefRefPtr<CefMediaAccessCallback> callback) {

    CEF_REQUIRE_UI_THREAD();
    std::cout << "[Security] Media access requested by: " << requesting_url.ToString() << std::endl;

    // Security Policy: Default Deny.
    // In a real application, we would send an IPC message to the UI to ask the user,
    // and hold the callback until the user responds.
    // For now, we strictly deny all camera/mic access automatically.

    callback->Cancel();
    return true; // Indicates we handled the request
}

bool SecurePermissionHandler::OnShowPermissionPrompt(
    CefRefPtr<CefBrowser> browser,
    uint64_t prompt_id,
    const CefString& requesting_origin,
    uint32_t requested_permissions,
    CefRefPtr<CefPermissionPromptCallback> callback) {

    CEF_REQUIRE_UI_THREAD();
    std::cout << "[Security] Permission prompt requested by: " << requesting_origin.ToString() << std::endl;

    // Similarly, default deny for Geolocation, Notifications, etc.
    callback->Continue(CEF_PERMISSION_RESULT_DENY);
    return true;
}

void SecurePermissionHandler::OnDismissPermissionPrompt(
    CefRefPtr<CefBrowser> browser,
    uint64_t prompt_id,
    cef_permission_request_result_t result) {
    CEF_REQUIRE_UI_THREAD();
    // Logic for cleanup after prompt disappears
}

#endif
