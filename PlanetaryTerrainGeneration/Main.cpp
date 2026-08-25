#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Util.h"
#include "Shader.h"
#include "Renderer.h"
#include "Camera.h"
#include "PostProcessor.h"

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

static void render() {
    if (isPixelated) {
        postProcessor.begin();
	    drawBackground();
        drawPlanet();
        postProcessor.render(pixelationShader);
	} else {
        postProcessor.end();
        drawBackground();
		drawPlanet();
	}

    drawDebugUI();
}

static void update(double delta) {
    updateCamera(delta);
}

int main() {
    if (init() != 0) return -1;

    initImGUI(window);
    initPlanet();
    initShaders();
    postProcessor.init(screenWidth, screenHeight);
    setupCallbacks(window);

    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentFrameTime = glfwGetTime();
        double delta = currentFrameTime - lastTime;
        lastTime = currentFrameTime;

        if (delta > 0.1) delta = 0.1;

        update(delta);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    postProcessor.cleanup();
    cleanup();
    return 0;
}