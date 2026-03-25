#pragma once

#include <glm/glm.hpp>

#include "CubeFace.h"

struct Planet {
    CubeFace faces[6];
    int resolution = 64;

    Planet() = default;
    Planet(int resolution);

    void build();
    void draw() const;
};

inline Planet planet;