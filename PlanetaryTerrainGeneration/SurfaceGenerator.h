#pragma once

#include <glm/glm.hpp>
#include "FastNoiseLite.h"
#include "PaletteGenerator.h"

struct NoiseParams {
	int seed = 24241;
	int resolution = 128;

	float height = 0.01f; // physical elevation multiplier

	bool applyLighting = false; // apply lighting effects
};

inline NoiseParams params;

class SurfaceGenerator {
private:
	FastNoiseLite simplex, domainWarp, ridged, cellular;
	float phases[3] = { 0.0f, 0.0f, 0.0f };

public:
	SurfaceGenerator();
	
	void seedGenerator();
    float generateElevation(int type, glm::vec3 position);
};

inline SurfaceGenerator surfaceGenerator;