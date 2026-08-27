#include "download_handler.h"
#include <iostream>
#include <algorithm>

#ifdef HAS_CEF
#include "include/wrapper/cef_helpers.h"

SecureDownloadHandler::SecureDownloadHandler() {}

bool SecureDownloadHandler::IsDangerousFile(const std::string& file_name) {
    // Extract extension
    size_t dot_pos = file_name.find_last_of('.');
    if (dot_pos == std::string::npos) return false;

    std::string ext = file_name.substr(dot_pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // List of highly dangerous extensions on Windows
    const std::vector<std::string> dangerous_exts = {
        "exe", "bat", "vbs", "ps1", "scr", "cmd", "msi", "com", "pif", "hta"
    };

    return std::find(dangerous_exts.begin(), dangerous_exts.end(), ext) != dangerous_exts.end();
}

void SecureDownloadHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefDownloadItem> download_item,
                                             const CefString& suggested_name,
                                             CefRefPtr<CefBeforeDownloadCallback> callback) {
    CEF_REQUIRE_UI_THREAD();

    std::string file_name = suggested_name.ToString();

    // Security check
    if (IsDangerousFile(file_name)) {
        std::cerr << "[Security Block] Blocked dangerous download: " << file_name << std::endl;
        // Do not proceed with download
        return;
    }

    std::cout << "[Download] Starting download for: " << file_name << std::endl;

    // Prompt the user for the download directory.
    // In a real browser, this would be a user preference or a Windows Save dialog.
    callback->Continue(file_name, true);
}

void SecureDownloadHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                              CefRefPtr<CefDownloadItem> download_item,
                                              CefRefPtr<CefDownloadItemCallback> callback) {
    CEF_REQUIRE_UI_THREAD();

    if (download_item->IsComplete()) {
        std::cout << "[Download] Completed: " << download_item->GetFullPath().ToString() << std::endl;
    } else if (download_item->IsCanceled()) {
        std::cout << "[Download] Canceled." << std::endl;
    } else {
        // Here we could send IPC to UI to update a download progress bar
        int percent = download_item->GetPercentComplete();
        // std::cout << "[Download] Progress: " << percent << "%" << std::endl;
    }
}
#endif
