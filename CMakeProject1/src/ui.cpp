#include "ui.h"
#include "imgui.h"
#include <filesystem>

void DiskUI::Render(const DiskInfo& info, GLFWwindow* window) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)windowHeight));

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Main Disk Panel", nullptr, windowFlags);

    ImGui::Text("System Storage Information");
    ImGui::Separator();
    ImGui::Text("Total Space: %.2f GB", info.totalDisk);
    ImGui::Text("Used Space: %.2f GB", info.usedDisk);
    ImGui::Text("Free Space: %.2f GB", info.emptyDisk);
    ImGui::ProgressBar(info.percentDisk / 100.0f, ImVec2(-1, 25), "Usage Rate");
    ImGui::Separator();

    if (scanner.IsScanning()) {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Scanning Entire Drive... Please Wait.");
        ImGui::Text("Current File: %s", scanner.GetCurrentScanPath().c_str());

        ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(-1, 15), "Analyzing...");
    }
    else {
        if (ImGui::Button("Deep Scan Entire C:\\ Drive", ImVec2(-1, 40))) {
            scanner.StartFullScan("C:\\");
        }

        ImGui::Separator();

        const auto& results = scanner.GetResults();

        if (results.at(FileCategory::Application).fileCount > 0 || results.at(FileCategory::Other).fileCount > 0) {

            ImGui::Text("Categorized Storage Breakdown:");
            ImGui::Spacing();

            for (const auto& [category, stats] : results) {
                float sizeInGB = stats.totalSize / (1024.0f * 1024.0f * 1024.0f);

                float ratio = (info.usedDisk > 0) ? (sizeInGB / info.usedDisk) : 0.0f;

                ImGui::Text("%s - %zu files", stats.name.c_str(), stats.fileCount);

                char buf[32];
                sprintf(buf, "%.2f GB", sizeInGB);
                ImGui::ProgressBar(ratio, ImVec2(-1, 20), buf);
                ImGui::Spacing();
            }
        }
    }

    ImGui::End();
}

DiskInfo DiskUI::ReadDiskStatus(const std::string& drivePath) {
    DiskInfo info;
    try {
        std::filesystem::space_info storage = std::filesystem::space(drivePath);
        float bytesToGB = 1024.0f * 1024.0f * 1024.0f;
        info.totalDisk = storage.capacity / bytesToGB;
        info.emptyDisk = storage.available / bytesToGB;
        info.usedDisk = info.totalDisk - info.emptyDisk;
        if (info.totalDisk > 0) info.percentDisk = (info.usedDisk / info.totalDisk) * 100.0f;
    }
    catch (...) {}
    return info;
}