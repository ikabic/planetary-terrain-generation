#pragma once

#include <glm/glm.hpp>

#include "CubeFace.h"
#include "Palette.h"

enum PlanetType {
	GAS_ICE_GIANT,
	TERRESTRIAL,
	AEOLIAN_FLUVIAL,
	CRATERED,
	VOLCANIC,
	GLACIAL,
	LAVA_WORLD,
	WATER_WORLD,
    ALIEN
};

class Planet {
    CubeFace faces[6];
    int resolution = 64;
	unsigned int seed = 0;

    float temperature = 0.5f;
    float size = 1.0f;

    bool hasAtmosphere = true;
    int numSatellites = 0;

    float rotationSpeed = 0.2f;
    float tiltAngle = 0.0f; // degrees

    PlanetType type;
    Palette palette;

public:
    Planet() = default;
    Planet(int resolution, unsigned int seed);

	Palette& getPalette() { return palette; }

    void build();
    void draw() const;

    void randomiseTraits();
};

inline Planet planet(128, 24241);