#ifndef SECURE_BROWSER_RENDER_APP_H_
#define SECURE_BROWSER_RENDER_APP_H_

#ifdef HAS_CEF
#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

// Renderer process application class. Crucial for injecting JS bindings.
class SecureRenderApp : public CefApp,
                        public CefRenderProcessHandler {
public:
    SecureRenderApp();

    // CefApp methods
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    // CefRenderProcessHandler methods
    void OnWebKitInitialized() override;

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context) override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

private:
    CefRefPtr<CefMessageRouterRendererSide> message_router_;

    IMPLEMENT_REFCOUNTING(SecureRenderApp);
};
#endif

#endif // SECURE_BROWSER_RENDER_APP_H_
