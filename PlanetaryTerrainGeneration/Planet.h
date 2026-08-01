#pragma once

#include <glm/glm.hpp>

#include "CubeFace.h"

class Planet {
    CubeFace faces[6];
    int resolution = 64;
	unsigned int seed = 0;

public:
    Planet() = default;
    Planet(int resolution, unsigned int seed);

    void build();
    void draw() const;
};

inline Planet planet;