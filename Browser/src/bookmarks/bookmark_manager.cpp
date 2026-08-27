#include "bookmark_manager.h"
#include "../crypto/secure_crypto.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#endif

BookmarkManager::BookmarkManager() {
    LoadBookmarks();
}

BookmarkManager::~BookmarkManager() {
    SaveBookmarks();
}

std::string BookmarkManager::GetBookmarksFilePath() const {
    std::string path = "bookmarks.enc"; // Fallback to encrypted file
#if defined(_WIN32)
    char szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        path = std::string(szPath) + "\\SecureBrowser\\bookmarks.enc";
    }
#endif
    return path;
}

void BookmarkManager::LoadBookmarks() {
    std::string decrypted_csv;
    if (!SecureCrypto::ReadEncryptedFile(GetBookmarksFilePath(), decrypted_csv)) return;

    std::stringstream file(decrypted_csv);
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string url, title;
        if (std::getline(ss, url, ',') && std::getline(ss, title)) {
            BookmarkEntry entry = {url, title};
            bookmarks_.push_back(entry);
        }
    }
}

void BookmarkManager::SaveBookmarks() const {
    std::stringstream csv;
    for (const auto& entry : bookmarks_) {
        csv << entry.url << "," << entry.title << "\n";
    }
    SecureCrypto::WriteEncryptedFile(GetBookmarksFilePath(), csv.str());
}

void BookmarkManager::AddBookmark(const std::string& url, const std::string& title) {
    // Security: Validate the URL before storing
    if (url.find("javascript:") == 0 || url.find("data:") == 0) {
        std::cerr << "[Security Block] Attempted to bookmark dangerous scheme." << std::endl;
        return;
    }

    // Check for duplicates
    for (const auto& b : bookmarks_) {
        if (b.url == url) {
            std::cout << "[Bookmark] Already bookmarked: " << url << std::endl;
            return;
        }
    }

    BookmarkEntry entry = { url, title };
    bookmarks_.push_back(entry);

    // Save immediately
    SaveBookmarks();

    std::cout << "[Bookmark] Added: " << title << " (" << url << ")" << std::endl;
}

void BookmarkManager::RemoveBookmark(const std::string& url) {
    auto it = std::remove_if(bookmarks_.begin(), bookmarks_.end(),
                             [&url](const BookmarkEntry& b) { return b.url == url; });

    if (it != bookmarks_.end()) {
        bookmarks_.erase(it, bookmarks_.end());
        SaveBookmarks();
        std::cout << "[Bookmark] Removed: " << url << std::endl;
    }
}

std::vector<BookmarkEntry> BookmarkManager::GetBookmarks() {
    return bookmarks_;
}
