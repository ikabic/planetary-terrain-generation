#pragma once

#include <glm/glm.hpp>

#include "CubeFace.h"
#include "Palette.h"

class Planet {
    CubeFace faces[6];
    int resolution = 64;
	unsigned int seed = 0;

    Palette palette;
public:
    Planet() = default;
    Planet(int resolution, unsigned int seed);

	Palette& getPalette() { return palette; }

    void build();
    void draw() const;

    void randomiseTraits();
};

inline Planet planet;