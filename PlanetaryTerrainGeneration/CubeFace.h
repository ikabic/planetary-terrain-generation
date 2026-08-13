#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class CubeFace {
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;

    void build(glm::vec3 localUp, int type);
    void draw() const;
    void drawDebugUI();
};