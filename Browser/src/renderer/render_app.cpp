#include "render_app.h"

#ifdef HAS_CEF

SecureRenderApp::SecureRenderApp() {}

void SecureRenderApp::OnWebKitInitialized() {
    // Configure the renderer-side message router to inject 'cefQuery' and 'cefQueryCancel'
    // into the V8 context of the web page. This is what enables UI -> Native IPC.
    CefMessageRouterConfig config;
    config.js_query_function = "cefQuery";
    config.js_cancel_function = "cefQueryCancel";

    message_router_ = CefMessageRouterRendererSide::Create(config);
}

void SecureRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefV8Context> context) {
    if (message_router_) {
        message_router_->OnContextCreated(browser, frame, context);
    }
}

void SecureRenderApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefV8Context> context) {
    if (message_router_) {
        message_router_->OnContextReleased(browser, frame, context);
    }
}

bool SecureRenderApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                               CefRefPtr<CefFrame> frame,
                                               CefProcessId source_process,
                                               CefRefPtr<CefProcessMessage> message) {
    if (message_router_) {
        return message_router_->OnProcessMessageReceived(browser, frame, source_process, message);
    }
    return false;
}

#endif
