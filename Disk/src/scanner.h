#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>

enum class FileCategory {
    Application,
    Media,
    Document,
    Junk,
    Other
};

struct CategoryStats {
    std::string name;
    uintmax_t   totalSize = 0;
    size_t      fileCount = 0;
};

struct FolderNode {
    std::string name;
    std::string fullPath;
    uintmax_t   totalSize = 0;
    size_t      fileCount = 0;
    std::vector<std::shared_ptr<FolderNode>> children;
    FolderNode* parent = nullptr;

    float treemapX = 0, treemapY = 0, treemapW = 0, treemapH = 0;
};

struct LargeFile {
    std::string  path;
    uintmax_t    size = 0;
    FileCategory category = FileCategory::Other;
};

class DiskScanner {
public:
    DiskScanner();
    ~DiskScanner();

    void StartFullScan(const std::string& rootPath);
    void StopScan();
    void Reset();

    bool  IsScanning()  const { return isScanning.load(); }
    float GetProgress() const { return progress.load(); }

    std::string GetCurrentScanPath() const {
        std::lock_guard<std::mutex> lock(pathMutex);
        return currentScanPath;
    }

    const std::map<FileCategory, CategoryStats>& GetResults()   const { return results; }
    const std::shared_ptr<FolderNode>& GetRootNode()  const { return rootNode; }
    const std::vector<LargeFile>& GetLargeFiles() const { return largeFiles; }

    size_t GetTotalFilesScanned() const { return totalFilesScanned.load(); }

private:
    void         ScanWorker(std::string rootPath);
    FileCategory CategorizeFile(const std::string& extension);

    std::map<FileCategory, CategoryStats> results;
    std::vector<LargeFile>               largeFiles;
    std::shared_ptr<FolderNode>          rootNode;

    mutable std::mutex pathMutex;
    mutable std::mutex resultsMutex;

    std::atomic<bool>   isScanning;
    std::atomic<bool>   shouldStop;
    std::atomic<float>  progress;
    std::atomic<size_t> totalFilesScanned;

    std::string currentScanPath;
    std::thread scanThread;
};