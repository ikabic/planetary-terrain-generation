#pragma once

#include <glm/glm.hpp>
#include "FastNoiseLite.h"
#include "PaletteGenerator.h"

struct NoiseParams {
	int type = 0; // 0: GasGiant, 1: Terrestrial, 2: Desert, 3: CrateredMoon
    int seed = 24241;
	int resolution = 128;
    int octaves = 14;

	int colorNum = 5;
	PaletteType paletteType = ANALOGOUS;

    bool useWarp = false;
	float warpStrength = 0.5f;

	float craterFrequency = 2.0f, craterDepthMultiplier = 1.0f, craterThreshold = 0.45f;

	float baseFrequency = 0.8f; // Base frequency for terrain generation
};

inline NoiseParams params;

class SurfaceGenerator {
private:
	FastNoiseLite simplex, domainWarp, ridged, cellular;

public:
	SurfaceGenerator();
	
	void seedGenerator();
    float generateElevation(glm::vec3 position);
};

inline SurfaceGenerator surfaceGenerator;