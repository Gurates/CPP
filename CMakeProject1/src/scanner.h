#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <atomic>
#include <thread>

enum class FileCategory {
    Application,
    Media,
    Document,
    Junk,
    Other
};

struct CategoryStats {
    std::string name;
    uintmax_t totalSize = 0;
    size_t fileCount = 0;
};

class DiskScanner {
public:
    DiskScanner();
    ~DiskScanner();

    void StartFullScan(const std::string& rootPath);

    bool IsScanning() const { return isScanning; }

    std::string GetCurrentScanPath() const { return currentScanPath; }

    const std::map<FileCategory, CategoryStats>& GetResults() const { return results; }

private:
    void ScanWorker(std::string rootPath);
    FileCategory CategorizeFile(const std::string& extension);

    std::map<FileCategory, CategoryStats> results;

    std::atomic<bool> isScanning;
    std::string currentScanPath;
    std::thread scanThread;
};