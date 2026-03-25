#include <glm/glm.hpp>
#include <vector>

#include "CubeFace.h"

void CubeFace::build(glm::vec3 localUp, int resolution) {
    glm::vec3 axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    glm::vec3 axisB = glm::cross(localUp, axisA);

    std::vector<float> verts;
    std::vector<unsigned> indices;

    for (int x = 0; x < resolution; x++) {
        for (int y = 0; y < resolution; y++) {
            glm::vec2 percent = glm::vec2(x, y) / (float)(resolution - 1);

            glm::vec3 pointOnUnitCube = localUp + (percent.x - 0.5f) * 2.0f * axisA + (percent.y - 0.5f) * 2.0f * axisB;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);

            verts.insert(verts.end(), { pointOnUnitSphere.x, pointOnUnitSphere.y, pointOnUnitSphere.z }); // position
            verts.insert(verts.end(), { pointOnUnitSphere.x, pointOnUnitSphere.y, pointOnUnitSphere.z }); // normal

            if (x < resolution - 1 && y < resolution - 1) {
                unsigned i = x * resolution + y;
                indices.insert(indices.end(), { i, i + resolution + 1, i + resolution });
                indices.insert(indices.end(), { i, i + 1, i + resolution + 1 });
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void CubeFace::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}