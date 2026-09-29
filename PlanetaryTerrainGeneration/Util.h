#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>#include <cstdint>

inline GLFWwindow* window = nullptr;
inline int screenWidth = 0;
inline int screenHeight = 0;

inline double lastTime = 0.0;

inline bool isPixelated = true;

inline int endProgram(const std::string& msg) {
    printf("ERROR: %s\n", msg.c_str());
    glfwTerminate();
    return -1;
}

inline void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

inline uint32_t hashSeed(const std::string& input) {
    uint32_t hash = 2166136261u;    // 32-bit FNV offset basis
    for (char c : input) {
        hash ^= static_cast<uint8_t>(c);
        hash *= 16777619u;  // 32-bit FNV prime
    }
    return hash;
}