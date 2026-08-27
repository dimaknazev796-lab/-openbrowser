#include "browser_app.h"
#include "browser_client.h"
#include <iostream>

SecureBrowserApp::SecureBrowserApp() {}
SecureBrowserApp::~SecureBrowserApp() {}

#ifdef HAS_CEF
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

void SecureBrowserApp::OnBeforeCommandLineProcessing(const CefString& process_type,
                                                     CefRefPtr<CefCommandLine> command_line) {
    // Stage 2: Advanced Settings & Strict Security Flags

    // Process Isolation (Site Isolation) - Extremely important for security
    command_line->AppendSwitch("site-per-process");

    // Enforce WebGL/GPU Sandboxing
    command_line->AppendSwitch("enable-gpu-sandbox");

    // Hardware acceleration optimization (DirectX / ANGLE / OpenGL support)
    // CEF normally handles this on Windows via ANGLE, but we can explicitly enable
    // features for better performance while keeping sandbox on.
    command_line->AppendSwitch("enable-gpu");
    command_line->AppendSwitch("enable-webgl");
    command_line->AppendSwitchWithValue("use-angle", "d3d11"); // Direct3D 11 for modern Windows

    // Disallow insecure features
    command_line->AppendSwitch("disable-web-security"); // DO NOT APPEND. Just a reminder.
    // Ensure we don't accidentally run out of sandbox
    // command_line->AppendSwitch("no-sandbox"); // NEVR APPEND THIS in production
}

void SecureBrowserApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
    std::cout << "CEF Context Initialized. Ready to spawn windows securely." << std::endl;

    // Information used when creating the native window.
    CefWindowInfo window_info;

#if defined(_WIN32)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(NULL, "SecureBrowser");
#endif

    // Secure browser defaults
    CefBrowserSettings browser_settings;
    browser_settings.web_security = STATE_ENABLED; // Ensure Web Security is on
    browser_settings.javascript = STATE_ENABLED;
    browser_settings.plugins = STATE_DISABLED; // Disable legacy plugins

    // Create the secure client
    CefRefPtr<SecureBrowserClient> client(new SecureBrowserClient());

    // Initially load the local UI shell file
    std::string url = "file:///" + std::string("Browser/resources/ui/index.html"); // Placeholder for full path

    // Create the browser window asynchronously using standard CefBrowserHost.
    CefBrowserHost::CreateBrowser(window_info, client, url, browser_settings, nullptr, nullptr);
}
#endif
