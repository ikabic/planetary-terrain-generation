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
    int resolution = 128;
	unsigned int seed = 0;

    float temperature = 0.5f;
    float size = 1.0f;

    int satelliteCount = 0;
    std::vector<Satellite> satellites;

    bool hasRings = false;
	int ringCount = 0;
    std::vector<Satellite> ringParticles;

    float rotationSpeed = 0.2f;
    float axialTilt = 0.0f; // axial tilt in degrees

	bool isAlien = false;
    PlanetType type;
    Palette palette;

public:
    Planet() = default;
    Planet(int resolution, unsigned int seed);

	float getSize() const { return size; }
	float getAxialTilt() const { return axialTilt; }
	float getRotationSpeed() const { return rotationSpeed; }
	Palette& getPalette() { return palette; }
    bool getHasRings() const { return hasRings; }
	std::vector<Satellite> getSatellites() const { return satellites; }
    std::vector<Satellite> getRingParticles() const { return ringParticles; }

    void build();
    void draw() const;
    void drawInstanced(GLsizei instanceCount) const;

    void randomiseTraits();
    void generateSatellites();
    void generateRings();
};

inline Planet planet(128, 24241);