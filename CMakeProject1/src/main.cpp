#include <GLFW/glfw3.h>
#include "engine.h"
#include "ui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {
    GLFWwindow* window = nullptr;

    if (!GraphicEngine::Start(window)) return -1;

    DiskUI diskUI;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DiskInfo currentData = diskUI.ReadDiskStatus("C:\\");
        diskUI.Render(currentData, window);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    GraphicEngine::Stop(window);
    return 0;
}