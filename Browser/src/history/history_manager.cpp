#include "history_manager.h"
#include "../crypto/secure_crypto.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#endif

HistoryManager::HistoryManager() {
    LoadHistory();
}

HistoryManager::~HistoryManager() {
    SaveHistory();
}

std::string HistoryManager::GetHistoryFilePath() const {
    std::string path = "history.enc"; // Fallback to encrypted file
#if defined(_WIN32)
    char szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        path = std::string(szPath) + "\\SecureBrowser\\history.enc";
    }
#endif
    return path;
}

void HistoryManager::LoadHistory() {
    std::string decrypted_csv;
    if (!SecureCrypto::ReadEncryptedFile(GetHistoryFilePath(), decrypted_csv)) return;

    std::stringstream file(decrypted_csv);
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string url, title, ts_str;
        if (std::getline(ss, url, ',') && std::getline(ss, title, ',') && std::getline(ss, ts_str)) {
            try {
                HistoryEntry entry = {url, title, std::stoll(ts_str)};
                entries_.push_back(entry);
            } catch (...) {}
        }
    }
}

void HistoryManager::SaveHistory() const {
    std::stringstream csv;
    for (const auto& entry : entries_) {
        csv << entry.url << "," << entry.title << "," << entry.timestamp << "\n";
    }
    SecureCrypto::WriteEncryptedFile(GetHistoryFilePath(), csv.str());
}

void HistoryManager::AddEntry(const std::string& url, const std::string& title) {
    // Security: Validate the URL before storing
    // Do not store internal tool URLs or data/javascript URIs
    if (url.find("file://") == 0 || url.find("data:") == 0 || url.find("javascript:") == 0) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();

    HistoryEntry entry = { url, title, timestamp };
    entries_.push_back(entry);

    // Save immediately for persistence
    SaveHistory();

    std::cout << "[History] Logged URL: " << url << std::endl;
}

std::vector<HistoryEntry> HistoryManager::GetRecentHistory(int limit) {
    std::vector<HistoryEntry> result;
    int count = 0;

    // Return newest first (reverse iteration)
    for (auto it = entries_.rbegin(); it != entries_.rend() && count < limit; ++it, ++count) {
        result.push_back(*it);
    }

    return result;
}

void HistoryManager::ClearHistory() {
    entries_.clear();
    SaveHistory();
    std::cout << "[History] History Cleared." << std::endl;
}
