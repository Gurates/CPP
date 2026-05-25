#include "engine.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool GraphicEngine::Start(GLFWwindow*& window) {
    if (!glfwInit()) return false;
    window = glfwCreateWindow(500, 720, "Effortless Disk Cleaner v1.0", NULL, NULL);
    if (!window) { glfwTerminate(); return false; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    return true;
}

void GraphicEngine::Stop(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}