#pragma once

#include <glm/glm.hpp>

#include "CubeFace.h"
#include "Palette.h"
#include "Satellite.h"

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
    std::vector<Satellite> satellites;

    float rotationSpeed = 0.2f;
    float axialTilt = 0.0f; // axial tilt in degrees

    PlanetType type;
    Palette palette;

public:
    Planet() = default;
    Planet(int resolution, unsigned int seed);

	float getSize() const { return size; }
	float getAxialTilt() const { return axialTilt; }
	float getRotationSpeed() const { return rotationSpeed; }
	Palette& getPalette() { return palette; }
	std::vector<Satellite> getSatellites() const { return satellites; }

    void build();
    void draw() const;

    void randomiseTraits();
    void generateSatellites();
};

inline Planet planet(128, 24241);