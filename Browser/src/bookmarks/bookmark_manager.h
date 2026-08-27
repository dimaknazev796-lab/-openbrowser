#ifndef SECURE_BROWSER_BOOKMARK_MANAGER_H_
#define SECURE_BROWSER_BOOKMARK_MANAGER_H_

#include <string>
#include <vector>

// Stage 11: Secure Bookmark Management
struct BookmarkEntry {
    std::string url;
    std::string title;
};

class BookmarkManager {
public:
    BookmarkManager();
    ~BookmarkManager();

    // Add a bookmark
    void AddBookmark(const std::string& url, const std::string& title);

    // Remove a bookmark by URL
    void RemoveBookmark(const std::string& url);

    // Retrieve all bookmarks
    std::vector<BookmarkEntry> GetBookmarks();

private:
    // Persistent storage methods
    std::string GetBookmarksFilePath() const;
    void LoadBookmarks();
    void SaveBookmarks() const;

    std::vector<BookmarkEntry> bookmarks_;
};

#endif // SECURE_BROWSER_BOOKMARK_MANAGER_H_
