#ifndef SECURE_BROWSER_HISTORY_MANAGER_H_
#define SECURE_BROWSER_HISTORY_MANAGER_H_

#include <string>
#include <vector>

// Stage 10: Secure History Management
struct HistoryEntry {
    std::string url;
    std::string title;
    long long timestamp; // Unix timestamp
};

class HistoryManager {
public:
    HistoryManager();
    ~HistoryManager();

    // Log a visited URL securely
    void AddEntry(const std::string& url, const std::string& title);

    // Retrieve history (limited to avoid memory bloat)
    std::vector<HistoryEntry> GetRecentHistory(int limit = 50);

    // Clear history
    void ClearHistory();

private:
    std::string GetHistoryFilePath() const;
    void LoadHistory();
    void SaveHistory() const;

    std::vector<HistoryEntry> entries_;
};

#endif // SECURE_BROWSER_HISTORY_MANAGER_H_
