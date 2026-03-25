#include "Planet.h"

static const glm::vec3 directions[6] = {
    { 0,  1,  0},
    { 0, -1,  0},
    {-1,  0,  0},
    { 1,  0,  0},
    { 0,  0,  1},
    { 0,  0, -1},
};

Planet::Planet(int resolution) : resolution(resolution) {}

void Planet::build() {
    for (int i = 0; i < 6; i++)
        faces[i].build(directions[i], resolution);
}

void Planet::draw() const {
    for (auto& face : faces)
        face.draw();
}