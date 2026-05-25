#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include "scanner.h"

struct DiskInfo {
    float totalDisk = 0.0f;
    float emptyDisk = 0.0f;
    float usedDisk = 0.0f;
    float percentDisk = 0.0f;
};

class DiskUI {
public:
    void Render(const DiskInfo& info, GLFWwindow* window);
    DiskInfo ReadDiskStatus(const std::string& drivePath);

private:
    DiskScanner scanner;
    bool isScanned = false;
};