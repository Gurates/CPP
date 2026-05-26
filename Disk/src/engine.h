#pragma once
#include <GLFW/glfw3.h>

class GraphicEngine {
public:
    static bool Start(GLFWwindow*& window);
    static void Stop(GLFWwindow* window);
};