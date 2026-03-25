#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    float radius = 3.0f;   // Distance from origin
    float yaw = 0.0f;      // Horizontal angle (degrees)
    float pitch = 0.0f;    // Vertical angle (degrees, clamped)

    float orbitSpeed = 60.0f; // Degrees per second when key held
    float zoomSpeed = 0.5f;
    float minRadius = 1.5f;
    float maxRadius = 10.0f;
    float pitchLimit = 89.0f;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera() = default;
    Camera(glm::vec3 startPos, glm::vec3 upVec, float startYaw, float startPitch);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const;

    void processOrbit(float deltaYaw, float deltaPitch);
    void processZoom(float delta);
};

inline Camera camera;

void updateCamera(double delta);
void setupCallbacks(GLFWwindow* window);