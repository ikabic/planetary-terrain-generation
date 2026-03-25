#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Util.h"
#include "Shader.h"
#include "Renderer.h"
#include "Camera.h"

static int init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    screenWidth = mode->width;
    screenHeight = mode->height;

    window = glfwCreateWindow(screenWidth, screenHeight, "Planet", monitor, NULL);
    if (window == NULL) return endProgram("Failed to create window.");
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) return endProgram("GLEW couldn't initialize.");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return 0;
}

static void initCamera() {
    camera = Camera(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
}

static void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawPlanet();
}

static void update(double delta) {
    updateCamera(delta);
}

int main() {
    if (init() != 0) return -1;

    initCamera();
    initShaders();
    setupCallbacks(window);

    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentFrameTime = glfwGetTime();
        double delta = currentFrameTime - lastTime;
        lastTime = currentFrameTime;

        update(delta);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - currentFrameTime < frameTime) {}
    }

    cleanup();
    return 0;
}