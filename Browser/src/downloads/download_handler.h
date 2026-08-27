#ifndef SECURE_BROWSER_DOWNLOAD_HANDLER_H_
#define SECURE_BROWSER_DOWNLOAD_HANDLER_H_

#ifdef HAS_CEF
#include "include/cef_download_handler.h"
#include <string>

// Stage 9: Secure Download Manager
class SecureDownloadHandler : public CefDownloadHandler {
public:
    SecureDownloadHandler();

    // Security Check: Analyzes file extension to block dangerous executables.
    bool IsDangerousFile(const std::string& file_name);

    // CefDownloadHandler methods
    void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDownloadItem> download_item,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback> callback) override;

    void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDownloadItem> download_item,
                           CefRefPtr<CefDownloadItemCallback> callback) override;

private:
    IMPLEMENT_REFCOUNTING(SecureDownloadHandler);
};
#endif

#endif // SECURE_BROWSER_DOWNLOAD_HANDLER_H_
