#ifndef SECURE_BROWSER_APP_H_
#define SECURE_BROWSER_APP_H_

#ifdef HAS_CEF
#include "include/cef_app.h"
#else
class CefApp {
public:
    virtual ~CefApp() = default;
};
class CefBrowserProcessHandler {
public:
    virtual ~CefBrowserProcessHandler() = default;
};
class CefString {};
class CefCommandLine {};
template<typename T> class CefRefPtr {
public:
    CefRefPtr(T* ptr = nullptr) : ptr_(ptr) {}
    T* operator->() { return ptr_; }
    operator bool() const { return ptr_ != nullptr; }
private:
    T* ptr_;
};
#endif

class SecureBrowserApp : public CefApp
#ifdef HAS_CEF
    , public CefBrowserProcessHandler
#endif
{
public:
    SecureBrowserApp();
    virtual ~SecureBrowserApp();

#ifdef HAS_CEF
    // CefApp methods
    void OnBeforeCommandLineProcessing(const CefString& process_type,
                                       CefRefPtr<CefCommandLine> command_line) override;

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // CefBrowserProcessHandler methods
    void OnContextInitialized() override;

    IMPLEMENT_REFCOUNTING(SecureBrowserApp);
#endif
};

#endif  // SECURE_BROWSER_APP_H_
