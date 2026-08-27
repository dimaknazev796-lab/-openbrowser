#ifndef SECURE_BROWSER_CLIENT_H_
#define SECURE_BROWSER_CLIENT_H_

#ifdef HAS_CEF
#include "include/cef_client.h"
#include "include/cef_life_span_handler.h"
#include "include/wrapper/cef_message_router.h"
#include <list>

// Stage 2: IPC Message Handler for secure communication
class SecureMessageRouterHandler : public CefMessageRouterBrowserSide::Handler {
public:
    SecureMessageRouterHandler() {}
    virtual ~SecureMessageRouterHandler() {}

    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64_t query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override;

private:
    IMPLEMENT_REFCOUNTING(SecureMessageRouterHandler);
};

class SecureBrowserClient : public CefClient,
                            public CefLifeSpanHandler {
public:
    SecureBrowserClient();
    ~SecureBrowserClient() override;

    // CefClient methods:
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;

    // IPC
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

    // CefLifeSpanHandler methods:
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    bool DoClose(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // Tab Manager Access
    class TabManager* GetTabManager() { return tab_manager_; }

private:
    std::list<CefRefPtr<CefBrowser>> browser_list_;

    // Pointer to Tab Manager
    class TabManager* tab_manager_;

    // Download Manager
    CefRefPtr<CefDownloadHandler> download_handler_;

    // IPC Message Router
    CefRefPtr<CefMessageRouterBrowserSide> message_router_;
    CefRefPtr<SecureMessageRouterHandler> message_handler_;

    IMPLEMENT_REFCOUNTING(SecureBrowserClient);
};
#else
// Mock class for Linux testing
class SecureBrowserClient {
public:
    SecureBrowserClient() {}
    ~SecureBrowserClient() {}
};
#endif

#endif  // SECURE_BROWSER_CLIENT_H_
