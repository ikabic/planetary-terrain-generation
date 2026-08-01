#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

#include "Camera.h"
#include "Util.h"

Camera::Camera(glm::vec3 startPos, glm::vec3 upVec, float startYaw, float startPitch): up(upVec), yaw(startYaw), pitch(startPitch) {
    radius = glm::length(startPos);
}

glm::vec3 Camera::getPosition() const {
    float yawR = glm::radians(yaw);
    float pitchR = glm::radians(pitch);
    return glm::vec3(radius * cos(pitchR) * sin(yawR), radius * sin(pitchR), radius * cos(pitchR) * cos(yawR));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(getPosition(), glm::vec3(0.0f), up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
}

void Camera::processOrbit(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    yaw = fmodf(yaw, 360.0f);
    pitch += deltaPitch;
    pitch = std::clamp(pitch, -pitchLimit, pitchLimit);
}

void Camera::processZoom(float delta) {
    radius -= delta * zoomSpeed;
    radius = std::clamp(radius, minRadius, maxRadius);
}

static void scrollCallback(GLFWwindow* /*w*/, double /*xoff*/, double yoff) {
    camera.processZoom((float)yoff);
}

void updateCamera(double delta) {
    float move = camera.orbitSpeed * (float)delta;

	//camera.processOrbit(move, 0.0f); // temporary auto rotation, adjust later

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) isPixelated = !isPixelated;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.processOrbit(-move, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.processOrbit(move, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.processOrbit(0.0f, move);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.processOrbit(0.0f, -move);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void setupCallbacks(GLFWwindow* w) {
    glfwSetScrollCallback(w, scrollCallback);
}