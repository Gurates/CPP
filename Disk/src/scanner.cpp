#include "scanner.h"
#include <algorithm>

namespace fs = std::filesystem;

DiskScanner::DiskScanner()
    : isScanning(false), shouldStop(false), progress(0.0f), totalFilesScanned(0)
{
    results[FileCategory::Application] = { "Applications (.exe, .dll)", 0, 0 };
    results[FileCategory::Media] = { "Media (.mp4, .jpg, .mp3)",  0, 0 };
    results[FileCategory::Document] = { "Documents (.pdf, .docx)",   0, 0 };
    results[FileCategory::Junk] = { "Junk Files (.tmp, .log)",   0, 0 };
    results[FileCategory::Other] = { "Other",                     0, 0 };
}

DiskScanner::~DiskScanner() {
    StopScan();
}

void DiskScanner::StopScan() {
    shouldStop = true;
    if (scanThread.joinable())
        scanThread.join();
    shouldStop = false;
}

void DiskScanner::Reset() {
    StopScan();
    for (auto& pair : results) {
        pair.second.totalSize = 0;
        pair.second.fileCount = 0;
    }
    largeFiles.clear();
    rootNode.reset();
    totalFilesScanned = 0;
    currentScannedSize = 0;
    progress = 0.0f;
    {
        std::lock_guard<std::mutex> lock(pathMutex);
        currentScanPath = "";
    }
}

void DiskScanner::StartFullScan(const std::string& rootPath) {
    if (isScanning) return;
    Reset();
    isScanning = true;

    if (scanThread.joinable())
        scanThread.join();

    scanThread = std::thread(&DiskScanner::ScanWorker, this, rootPath);
}

void DiskScanner::ScanWorker(std::string rootPath) {
    auto options = fs::directory_options::skip_permission_denied;

    auto root = std::make_shared<FolderNode>();
    root->name = rootPath;
    root->fullPath = rootPath;

    std::map<std::string, std::shared_ptr<FolderNode>> folderMap;
    folderMap[rootPath] = root;

    std::map<FileCategory, CategoryStats> localResults = results;
    std::vector<LargeFile>               localLargeFiles;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath, options)) {
            if (shouldStop) break;

            if (totalFilesScanned % 100 == 0) {
                std::lock_guard<std::mutex> lock(pathMutex);
                currentScanPath = entry.path().string();
            }

            if (fs::is_directory(entry.status())) {
                std::string dirPath = entry.path().string();
                if (folderMap.find(dirPath) == folderMap.end()) {
                    auto node = std::make_shared<FolderNode>();
                    node->name = entry.path().filename().string();
                    node->fullPath = dirPath;

                    std::string parentPath = entry.path().parent_path().string();
                    auto parentIt = folderMap.find(parentPath);
                    if (parentIt != folderMap.end()) {
                        node->parent = parentIt->second.get();
                        parentIt->second->children.push_back(node);
                    }
                    folderMap[dirPath] = node;
                }
                continue;
            }

            if (!fs::is_regular_file(entry.status())) continue;

            uintmax_t fileSize = 0;
            try { fileSize = fs::file_size(entry.path()); }
            catch (...) { continue; }

            currentScannedSize += fileSize;

            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            FileCategory cat = CategorizeFile(ext);
            localResults[cat].totalSize += fileSize;
            localResults[cat].fileCount += 1;
            totalFilesScanned++;

            std::string dirPath = entry.path().parent_path().string();
            auto folderIt = folderMap.find(dirPath);
            while (folderIt != folderMap.end()) {
                folderIt->second->totalSize += fileSize;
                folderIt->second->fileCount++;
                folderIt->second->categorySizes[static_cast<int>(cat)] += fileSize;

                if (folderIt->second->parent) {
                    dirPath = folderIt->second->parent->fullPath;
                    folderIt = folderMap.find(dirPath);
                }
                else {
                    break;
                }
            }

            if (fileSize > 50ULL * 1024 * 1024)
                localLargeFiles.push_back({ entry.path().string(), fileSize, cat });
        }
    }
    catch (...) {}

    std::sort(localLargeFiles.begin(), localLargeFiles.end(),
        [](const LargeFile& a, const LargeFile& b) { return a.size > b.size; });
    if (localLargeFiles.size() > 100)
        localLargeFiles.resize(100);

    {
        std::lock_guard<std::mutex> lock(resultsMutex);
        results = localResults;
        largeFiles = localLargeFiles;
        rootNode = root;
    }

    {
        std::lock_guard<std::mutex> lock(pathMutex);
        currentScanPath = shouldStop ? "Scan stopped." : "Scan complete!";
    }

    progress = 1.0f;
    isScanning = false;
}
FileCategory DiskScanner::CategorizeFile(const std::string& extension) {
    if (extension == ".exe" || extension == ".dll" || extension == ".msi" ||
        extension == ".sys" || extension == ".com" || extension == ".bat")
        return FileCategory::Application;

    if (extension == ".mp4" || extension == ".mkv" || extension == ".avi" ||
        extension == ".mov" || extension == ".wmv" || extension == ".png" ||
        extension == ".jpg" || extension == ".jpeg" || extension == ".gif" ||
        extension == ".bmp" || extension == ".webp" || extension == ".mp3" ||
        extension == ".flac" || extension == ".wav" || extension == ".aac")
        return FileCategory::Media;

    if (extension == ".pdf" || extension == ".docx" || extension == ".doc" ||
        extension == ".txt" || extension == ".xlsx" || extension == ".xls" ||
        extension == ".pptx" || extension == ".csv" || extension == ".odt")
        return FileCategory::Document;

    if (extension == ".tmp" || extension == ".log" || extension == ".bak" ||
        extension == ".old" || extension == ".cache" || extension == ".dmp" ||
        extension == ".etl" || extension == ".temp")
        return FileCategory::Junk;

    return FileCategory::Other;
}