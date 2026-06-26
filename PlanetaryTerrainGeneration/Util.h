#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

inline GLFWwindow* window = nullptr;
inline int screenWidth = 0;
inline int screenHeight = 0;

inline double lastTime = 0.0;

inline int endProgram(const std::string& msg) {
    printf("ERROR: %s\n", msg.c_str());
    glfwTerminate();
    return -1;
}

inline void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}