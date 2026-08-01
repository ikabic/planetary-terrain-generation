#include <glm/gtc/constants.hpp>

#include "SurfaceGenerator.h"
#include "Randomiser.h"

SurfaceGenerator::SurfaceGenerator() {
	seedGenerator();

    simplex.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    simplex.SetFractalType(FastNoiseLite::FractalType_FBm);
    simplex.SetFractalOctaves(3);
    simplex.SetFrequency(0.88f);

    domainWarp.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    domainWarp.SetFrequency(1.5f);

    ridged.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    ridged.SetFractalType(FastNoiseLite::FractalType_Ridged);
    ridged.SetFractalOctaves(2);
    ridged.SetFrequency(2.0f);

    cellular.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    cellular.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
}

void SurfaceGenerator::seedGenerator() {
	uint32_t seed = randomiser.deriveSeed("terrain");

	simplex.SetSeed(seed);
	domainWarp.SetSeed(seed);
	ridged.SetSeed(seed);
	cellular.SetSeed(seed);
}

float SurfaceGenerator::generateElevation(glm::vec3 position) {
	seedGenerator();

    glm::vec3 p = position;
    int type = params.type; // Default type, can be changed based on context

	auto phase = []() { return randomiser.floatRange(0, glm::two_pi<float>()); };

    switch (type) {
    case 0: { // GasGiant
        float warpX = domainWarp.GetNoise(p.x, p.y, p.z);
        float warpStrenght = params.warpStrength;       // 0.01 - 0.1

        // Latitudinal sine wave bands along Y-axis
        float lat = p.y + warpX * warpStrenght;
        float bands = (std::sin(lat * 11.0f + phase()) + std::sin(lat * 15.0f + phase()) + std::sin(lat * 27.0f + phase())) * 0.5f + 0.5f;

        // Cloud turbulence detail
        float cloudDetail = simplex.GetNoise(p.x * 2.5f, p.y * 2.5f, p.z * 2.5f) * 0.5f + 0.5f;

        float bandsWeight = 0.7f, cloudWeight = 0.3f;
        return glm::clamp(bands * bandsWeight + cloudDetail * cloudWeight, 0.0f, 1.0f);
    }

    case 1: { // Terrestrial
        float warpX = domainWarp.GetNoise(p.x, p.y, p.z);
        float warpY = domainWarp.GetNoise(p.y, p.z, p.x);
        float warpStrenght = 0.062f;

        // Continental base
        float baseNoise = simplex.GetNoise(p.x + warpX * warpStrenght, p.y + warpY * warpStrenght, p.z) * 0.5f + 0.5f;
        baseNoise = std::pow(baseNoise, 1.2f);

        // Mountain ridges
        warpStrenght = 0.77f;
        float mountainNoise = ridged.GetNoise(p.x + warpX * warpStrenght, p.y + warpY * warpStrenght, p.z) * 0.5f + 0.5f;

        float height = baseNoise;
        float oceanThreshold = 0.45f, mountainHeight = 0.44f;

        if (baseNoise > oceanThreshold) // Add mountains if over ocean threshold
            height += (mountainNoise * mountainHeight) * (baseNoise - oceanThreshold);

        return glm::clamp(height, 0.0f, 1.0f);
    }

    case 2: { // Desert
        float warpStrenght = 0.8f;
        float warpX = domainWarp.GetNoise(p.x * warpStrenght, p.y * warpStrenght, p.z * warpStrenght);

        float baseFrequency = 1.5f;
        float baseTerrain = simplex.GetNoise(p.x * baseFrequency, p.y * baseFrequency, p.z * baseFrequency) * 0.5f + 0.5f;

        float canyon = ridged.GetNoise(p.x + warpX, p.y + warpX, p.z) * 0.5f + 0.5f;

        float baseWeight = 0.7f, canyonWeight = 0.3f;
        return glm::clamp(baseTerrain * baseWeight + canyon * canyonWeight, 0.0f, 1.0f);
    }

    case 3: { // CrateredMoon
        float baseFrequency = 2.7f;
        float base = simplex.GetNoise(p.x * baseFrequency, p.y * baseFrequency, p.z * baseFrequency) * 0.5f + 0.5f;

        float craterFrequency = 1.46f;
        float craters = cellular.GetNoise(p.x * craterFrequency, p.y * craterFrequency, p.z * craterFrequency) * 0.5f + 0.5f;

        float craterThreshold = 0.3f;
        float craterDepth = std::max(0.0f, craterThreshold - std::abs(craters));
        return glm::clamp(base - craterDepth, 0.0f, 1.0f);
    }
    }

    return 0.0f;
}