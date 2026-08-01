#include <glm/glm.hpp>
#include <vector>

#include "CubeFace.h"
#include "SurfaceGenerator.h"

void CubeFace::build(glm::vec3 localUp) {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }

    glm::vec3 axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    glm::vec3 axisB = glm::cross(localUp, axisA);

    std::vector<float> verts;
    std::vector<unsigned> indices;

    for (int x = 0; x < params.resolution; x++) {
        for (int y = 0; y < params.resolution; y++) {
            glm::vec2 percent = glm::vec2(x, y) / (float)(params.resolution - 1);

            glm::vec3 pointOnUnitCube = localUp + (percent.x - 0.5f) * 2.0f * axisA + (percent.y - 0.5f) * 2.0f * axisB;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);

            float rawElevation = surfaceGenerator.generateElevation(pointOnUnitSphere);

            float physicalElevation, seaCutoff = 0.45f;
            if (rawElevation > seaCutoff) physicalElevation = 1.0f + (rawElevation - seaCutoff) * 0.01f;
            else physicalElevation = 1.0f;

            glm::vec3 displacedPosition = pointOnUnitSphere * physicalElevation;

            verts.insert(verts.end(), { displacedPosition.x, displacedPosition.y, displacedPosition.z }); // position
            verts.push_back(rawElevation); // elevation

            if (x < params.resolution - 1 && y < params.resolution - 1) {
                unsigned i = x * params.resolution + y;
                indices.insert(indices.end(), { i, i + params.resolution + 1, i + params.resolution });
                indices.insert(indices.end(), { i, i + 1, i + params.resolution + 1 });
            }
        }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void CubeFace::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}