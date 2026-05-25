#include "scanner.h"
#include <algorithm>

namespace fs = std::filesystem;

DiskScanner::DiskScanner() : isScanning(false) {
    results[FileCategory::Application] = { "Applications (.exe, .dll)", 0, 0 };
    results[FileCategory::Media] = { "Media (.mp4, .jpg, .mp3)", 0, 0 };
    results[FileCategory::Document] = { "Documents (.pdf, .docx)", 0, 0 };
    results[FileCategory::Junk] = { "Junk Files (.tmp, .log)", 0, 0 };
    results[FileCategory::Other] = { "Other Files", 0, 0 };
}

DiskScanner::~DiskScanner() {
    if (scanThread.joinable()) {
        scanThread.join();
    }
}

void DiskScanner::StartFullScan(const std::string& rootPath) {
    if (isScanning) return;

    for (auto& pair : results) {
        pair.second.totalSize = 0;
        pair.second.fileCount = 0;
    }

    isScanning = true;

    if (scanThread.joinable()) {
        scanThread.join();
    }

    scanThread = std::thread(&DiskScanner::ScanWorker, this, rootPath);
}

void DiskScanner::ScanWorker(std::string rootPath) {
    try {
        auto options = fs::directory_options::skip_permission_denied;

        for (const auto& entry : fs::recursive_directory_iterator(rootPath, options)) {
            if (fs::is_regular_file(entry.status())) {

                currentScanPath = entry.path().string();

                uintmax_t fileSize = fs::file_size(entry.path());
                std::string ext = entry.path().extension().string();

                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                FileCategory cat = CategorizeFile(ext);
                results[cat].totalSize += fileSize;
                results[cat].fileCount += 1;
            }
        }
    }
    catch (...) {
    }

    currentScanPath = "Scan Completed!";
    isScanning = false;
}

FileCategory DiskScanner::CategorizeFile(const std::string& extension) {
    if (extension == ".exe" || extension == ".dll" || extension == ".msi" || extension == ".sys") {
        return FileCategory::Application;
    }
    else if (extension == ".mp4" || extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".mp3" || extension == ".mkv") {
        return FileCategory::Media;
    }
    else if (extension == ".pdf" || extension == ".docx" || extension == ".txt" || extension == ".xlsx") {
        return FileCategory::Document;
    }
    else if (extension == ".tmp" || extension == ".log" || extension == ".bak" || extension == ".old") {
        return FileCategory::Junk;
    }
    return FileCategory::Other;
}