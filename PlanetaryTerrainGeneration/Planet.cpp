#include "Planet.h"
#include "PaletteManager.h"
#include "Randomiser.h"

static const glm::vec3 directions[6] = {
    { 0,  1,  0},
    { 0, -1,  0},
    {-1,  0,  0},
    { 1,  0,  0},
    { 0,  0,  1},
    { 0,  0, -1},
};

Planet::Planet(int resolution, unsigned int seed) : resolution(resolution), seed(seed) {}

void Planet::build() {
	randomiseTraits();

    for (int i = 0; i < 6; i++)
        faces[i].build(directions[i], type);

	generateSatellites();
	if (hasRings) generateRings();
}

void Planet::draw() const {
    for (auto& face : faces)
        face.draw();
}

void Planet::drawInstanced(GLsizei instanceCount) const {
    for (auto& face : faces)
        face.drawInstanced(instanceCount);
}

void Planet::randomiseTraits() {
    randomiser.deriveSeed("traits");

	constexpr float minSize = 0.9f, maxSize = 1.1f;                                                         // size params
	constexpr float alienProbability = 0.05f;                                                               // type params
	constexpr int maxSatellites = 8;                                                                        // satellite params
	constexpr float minSpeed = 0.05f, maxSpeed = 0.4f;                                                      // rotation speed params
	constexpr float minTilt = 0.0f, maxTilt = 45.0f, extremeTilt = 180.0f, extremeTiltProbability = 0.05f;  // axial tilt params
	constexpr float baseRingChance = 0.35f, perSatellitePenalty = 0.05f;                                    // ring params

    // Root traits
    size = randomiser.floatRange(minSize, maxSize);
	temperature = randomiser.floatRange(0.0f, 1.0f);
	type = randomiser.chance(alienProbability) ? ALIEN : static_cast<PlanetType>(randomiser.intRange(0, 7)); // chance to roll a completely random alien planet

    float normalisedSize = (size - minSize) / (maxSize - minSize);

	// Derived traits
	palette = paletteManager.get(type, temperature);

    int maxForSize = static_cast<int>(std::round(normalisedSize * maxSatellites)); // satellite count based on size
    satelliteCount = randomiser.intRange(0, maxForSize);
    
    float speedCeiling = maxSpeed - (satelliteCount / float(maxSatellites)) * 0.2f; // rotation speed based on satellites (more moons, lower ceiling)
    rotationSpeed = randomiser.floatRange(minSpeed, speedCeiling);
    
    // axial tilt based on satellite count and chance for extreme tilt
	axialTilt = satelliteCount == 0 || randomiser.chance(extremeTiltProbability) ? randomiser.floatRange(minTilt, extremeTilt) : randomiser.floatRange(minTilt, maxTilt, 2.0f);
	
    hasAtmosphere = randomiser.chance(normalisedSize); // atmosphere based on size
	// roll for athmosphere thickness and pattern based on temp and rotation speed

    float ringChance = baseRingChance - satelliteCount * perSatellitePenalty;
    hasRings = randomiser.chance(std::max(0.0f, ringChance));
}

void Planet::generateSatellites() {
    randomiser.deriveSeed("satellite");
    satellites.clear();
    satellites.resize(satelliteCount);
}

void Planet::generateRings() {
    randomiser.deriveSeed("ring");
    ringParticles.clear();

	ringCount = randomiser.intRange(1, 2);
    float sharedTilt = glm::radians(randomiser.floatRange(10.0f, 13.0f));

	float currentInnerRadius = size + 0.35f;

    for (int r = 0; r < ringCount; r++) {
        int particlesInRing = randomiser.intRange(200, 300);
        float ringWidth = ringCount == 1 ? randomiser.floatRange(0.5f, 0.7f) : randomiser.floatRange(0.15f, 0.25f);
        float gapSize = randomiser.floatRange(0.2f, 0.4f);

        float innerRadius = currentInnerRadius;
        float outerRadius = innerRadius + ringWidth;
        glm::vec3 ringColour = getBaseColour();

        for (int i = 0; i < particlesInRing; i++)
            ringParticles.emplace_back(sharedTilt, innerRadius, outerRadius, ringColour);

        currentInnerRadius = outerRadius + gapSize;
    }
}