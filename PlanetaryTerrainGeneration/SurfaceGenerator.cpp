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
    uint32_t seed = randomiser.getSeed();

	simplex.SetSeed(seed);
	domainWarp.SetSeed(seed);
	ridged.SetSeed(seed);
	cellular.SetSeed(seed);

	for (int i = 0; i < 3; i++) phases[i] = randomiser.floatRange(0.0f, glm::two_pi<float>());
}

float SurfaceGenerator::generateElevation(int type, glm::vec3 pos) {
    switch (type) {
    case 0: {   // Gas/ice giants
        float warpX = domainWarp.GetNoise(pos.x, pos.y, pos.z);
        float warpAmp = randomiser.floatRange(0.01f, 0.055f);

        // Latitudinal sine wave bands along Y-axis
        float lat = pos.y + warpX * warpAmp;
        float bands = (std::sin(lat * 11.0f + phases[0]) + std::sin(lat * 15.0f + phases[1]) + std::sin(lat * 27.0f + phases[2])) * 0.5f + 0.5f;

        // Cloud turbulence detail
        float cloudDetail = simplex.GetNoise(pos.x * 2.5f, pos.y * 2.5f, pos.z * 2.5f) * 0.5f + 0.5f;

        float bandsWeight = 0.7f, cloudWeight = 0.3f;
        return glm::clamp(bands * bandsWeight + cloudDetail * cloudWeight, 0.0f, 1.0f);
    }

    case 1: {   // Tectonic/terrestrial planets
        float warpX = domainWarp.GetNoise(pos.x, pos.y, pos.z);
        float warpY = domainWarp.GetNoise(pos.y, pos.z, pos.x);
        float warpAmp = 0.062f;

        // Continental base
        float baseNoise = simplex.GetNoise(pos.x + warpX * warpAmp, pos.y + warpY * warpAmp, pos.z) * 0.5f + 0.5f;
        baseNoise = std::pow(baseNoise, 1.2f);

        // Mountain ridges
        warpAmp = 0.77f;
        float mountainNoise = ridged.GetNoise(pos.x + warpX * warpAmp, pos.y + warpY * warpAmp, pos.z) * 0.5f + 0.5f;

        float height = baseNoise;
        float oceanThreshold = 0.45f, mountainHeight = 0.44f;

        if (baseNoise > oceanThreshold) // Add mountains if over ocean threshold
            height += (mountainNoise * mountainHeight) * (baseNoise - oceanThreshold);

        return glm::clamp(height, 0.0f, 1.0f);
    }

    case 2: {   // Aeolian/fluvial planets
        float warpFrequency = 0.8f;
        float warpXY = domainWarp.GetNoise(pos.x * warpFrequency, pos.y * warpFrequency, pos.z * warpFrequency);

        float baseFrequency = 1.5f;
        float baseTerrain = simplex.GetNoise(pos.x * baseFrequency, pos.y * baseFrequency, pos.z * baseFrequency) * 0.5f + 0.5f;

        float canyon = ridged.GetNoise(pos.x + warpXY, pos.y + warpXY, pos.z) * 0.5f + 0.5f;

        float baseWeight = 0.7f, canyonWeight = 0.3f;
        return glm::clamp(baseTerrain * baseWeight + canyon * canyonWeight, 0.0f, 1.0f);
    }

    case 3: {   // Cratered planets
        float baseFrequency = 2.7f;
        float base = simplex.GetNoise(pos.x * baseFrequency, pos.y * baseFrequency, pos.z * baseFrequency) * 0.5f + 0.5f;

        float craterFrequency = 1.46f;
        float craters = cellular.GetNoise(pos.x * craterFrequency, pos.y * craterFrequency, pos.z * craterFrequency) * 0.5f + 0.5f;

        float craterThreshold = 0.3f;
        float craterDepth = std::max(0.0f, craterThreshold - std::abs(craters));
        return glm::clamp(base - craterDepth, 0.0f, 1.0f);
    }

    case 4: {   // Volcanic planets
        float baseFrequency = 1.1f;
        float basePlain = simplex.GetNoise(pos.x * baseFrequency, pos.y * baseFrequency, pos.z * baseFrequency) * 0.5f + 0.5f;
        basePlain = std::pow(basePlain, 1.6f);

        float domeFrequency = 2.2f;
        float domes = cellular.GetNoise(pos.x * domeFrequency, pos.y * domeFrequency, pos.z * domeFrequency) * 0.5f + 0.5f;

        float domeThreshold = 0.4f;
        float domeHeight = std::max(0.0f, domeThreshold - std::abs(domes));

		float domeDetailFrequency = 3.0f;
        float domeDetail = ridged.GetNoise(pos.x * domeDetailFrequency, pos.y * domeDetailFrequency, pos.z * domeDetailFrequency) * 0.5f + 0.5f;

        float height = basePlain + domeHeight * (1.0f + domeDetail * 0.4f);
        return glm::clamp(height, 0.0f, 1.0f);
    }

    case 5: {   // Glacial/ice planets
        float plainFrequency = 0.9f;
        float convectionCells = cellular.GetNoise(pos.x * plainFrequency, pos.y * plainFrequency, pos.z * plainFrequency) * 0.5f + 0.5f;
        float smoothPlain = std::pow(convectionCells, 3.0f) * 0.15f;

        float maskFrequency = 0.6f;
        float mountainMask = simplex.GetNoise(pos.x * maskFrequency, pos.y * maskFrequency, pos.z * maskFrequency) * 0.5f + 0.5f;

        float mountainThreshold = 0.55f;
        float iceMountains = ridged.GetNoise(pos.x * 2.2f, pos.y * 2.2f, pos.z * 2.2f) * 0.5f + 0.5f;

        float height = smoothPlain;
        if (mountainMask > mountainThreshold)
            height += iceMountains * (mountainMask - mountainThreshold) * 1.6f;

        return glm::clamp(height, 0.0f, 1.0f);
    }

    case 6: {   // Lava worlds
        float warpFrequency = 0.65f, warpAmp = 0.55f;
        glm::vec3 warpedPos = pos + glm::vec3(
            simplex.GetNoise(pos.x * warpFrequency + 0.0f, pos.y * warpFrequency + 0.0f, pos.z * warpFrequency + 0.0f),
            simplex.GetNoise(pos.x * warpFrequency + 17.3f, pos.y * warpFrequency + 17.3f, pos.z * warpFrequency + 17.3f),
            simplex.GetNoise(pos.x * warpFrequency + 43.1f, pos.y * warpFrequency + 43.1f, pos.z * warpFrequency + 43.1f)
        ) * warpAmp;

        // Primary crack network
        float crackFrequency = 1.35f * 100.0f; // * 100.0f to cancel out the default 0.01f frequency of FastNoiseLite
        float cracks = cellular.GetNoise(warpedPos.x * crackFrequency, warpedPos.y * crackFrequency, warpedPos.z * crackFrequency) * 0.5f + 0.5f;
        float primaryRifts = std::pow(1.0f - cracks, 2.9f);

        // Branching fissures
        float fissureFrequency = 1.5f;
        float fineCracks = ridged.GetNoise(warpedPos.x * fissureFrequency, warpedPos.y * fissureFrequency, warpedPos.z * fissureFrequency) * 0.5f + 0.5f;
        fineCracks = std::pow(fineCracks, 3.0f) * 0.9f;

        float oceanFrequency = 0.35f;
        float oceanMask = simplex.GetNoise(pos.x * oceanFrequency, pos.y * oceanFrequency, pos.z * oceanFrequency) * 0.5f + 0.5f;

        // Crust baseline
        float crustFrequency = 1.8f;
        float crustRoughness = simplex.GetNoise(pos.x * crustFrequency, pos.y * crustFrequency, pos.z * crustFrequency) * 0.5f + 0.5f;
        float baseCrust = crustRoughness * 0.08f;

        float totalRifts = glm::clamp(primaryRifts + fineCracks, 0.0f, 1.0f);
        float height = baseCrust + totalRifts * 0.80f;

        float oceanThreshold = 0.40f;
        if (oceanMask < oceanThreshold) {
            float oceanFactor = (oceanThreshold - oceanMask) / oceanThreshold;
            height = glm::mix(height, 0.95f, oceanFactor * 0.90f);
        }

        return glm::clamp(height, 0.0f, 1.0f);

    }

    case 7: {   // Water worlds
        float warpX = domainWarp.GetNoise(pos.x, pos.y, pos.z);
        float warpY = domainWarp.GetNoise(pos.y, pos.z, pos.x);
        float warpAmp = 0.05f;

        float baseNoise = simplex.GetNoise(pos.x + warpX * warpAmp, pos.y + warpY * warpAmp, pos.z) * 0.5f + 0.5f;
        baseNoise = std::pow(baseNoise, 2.4f);

        float rippleFrequency = 6.0f;
        float ripples = simplex.GetNoise(pos.x * rippleFrequency, pos.y * rippleFrequency, pos.z * rippleFrequency) * 0.5f + 0.5f;

        float seaLevel = 0.82f;
        float height = baseNoise * 0.35f + ripples * 0.05f;

        if (baseNoise > seaLevel)
            height += (baseNoise - seaLevel) * 3.0f;

        return glm::clamp(height, 0.0f, 1.0f);
    }
    }

    return 0.0f;
}