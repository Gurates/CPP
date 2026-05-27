#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <chrono>
#include <memory>
#include "scanner.h"

struct DiskInfo {
    float totalDisk = 0.0f;
    float freeDisk = 0.0f;
    float usedDisk = 0.0f;
    float usagePercent = 0.0f;
};

struct RamInfo {
    float totalPhysical = 0.0f;
    float usedPhysical = 0.0f;
    float freePhysical = 0.0f;
    float usagePercent = 0.0f;
};

enum class AppScreen {
    Overview,
    FileTree,
    LargeFiles,
    Junk,
    Ram,
};

class DiskUI {
public:
    DiskUI();
    void Render(GLFWwindow* window);
    void RefreshDiskInfo(const std::string& drivePath);

private:
    void RenderTopBar(float windowWidth);
    void RenderOverview(float windowWidth, float windowHeight);
    void RenderFileTree(float windowWidth, float windowHeight);
    void RenderLargeFiles(float windowWidth, float windowHeight);
    void RenderJunk(float windowWidth, float windowHeight);
    void RenderRam(float windowWidth, float windowHeight);

    void              LayoutTreemap(FolderNode* node, float x, float y, float w, float h);
    void              DrawTreemap(const FolderNode* node, float mouseX, float mouseY);
    const FolderNode* FindHoveredNode(const FolderNode* node, float mx, float my);

    DiskScanner scanner;
    DiskInfo    diskInfo;
    RamInfo     ramInfo;
    AppScreen   currentScreen = AppScreen::Overview;

    std::chrono::steady_clock::time_point lastDiskRefresh;
    bool diskInfoValid = false;

    FolderNode* selectedFolder = nullptr;
    bool        confirmDeleteOpen = false;
};