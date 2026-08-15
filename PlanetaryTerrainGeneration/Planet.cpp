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
}

void Planet::draw() const {
    for (auto& face : faces)
        face.draw();
}

void Planet::randomiseTraits() {
    randomiser.deriveSeed("traits");
	constexpr float minSize = 0.9f, maxSize = 1.1f;                                                         // size params
	constexpr float alienProbability = 0.05f;                                                               // type params
	constexpr int maxSatellites = 8;                                                                        // satellite params
	constexpr float minSpeed = 0.05f, maxSpeed = 0.4f;                                                      // rotation speed params
	constexpr float minTilt = 0.0f, maxTilt = 45.0f, extremeTilt = 180.0f, extremeTiltProbability = 0.1f;   // axial tilt params

    // Root traits
    size = randomiser.floatRange(minSize, maxSize);
	temperature = randomiser.floatRange(0.0f, 1.0f);
	type = randomiser.chance(alienProbability) ? ALIEN : static_cast<PlanetType>(randomiser.intRange(0, 7)); // chance to roll a completely random alien planet

    float normalisedSize = (size - minSize) / (maxSize - minSize);

	// Derived traits
	palette = paletteManager.get(type, temperature);

    int maxForSize = static_cast<int>(std::round(normalisedSize * maxSatellites)); // satellite count based on size
    numSatellites = randomiser.intRange(0, maxForSize);
    
    float speedCeiling = maxSpeed - (numSatellites / float(maxSatellites)) * 0.2f; // rotation speed based on satellites (more moons, lower ceiling)
    rotationSpeed = randomiser.floatRange(minSpeed, speedCeiling);
    
    // axial tilt based on satellite count and chance for extreme tilt
	axialTilt = numSatellites == 0 || randomiser.chance(extremeTiltProbability) ? randomiser.floatRange(minTilt, extremeTilt) : randomiser.floatRange(minTilt, maxTilt); 
	
    hasAtmosphere = randomiser.chance(normalisedSize); // atmosphere based on size
	// roll for athmosphere thickness and pattern based on temp and rotation speed

	// roll for rings based on size, satellites and temp
}

void Planet::generateSatellites() {
    randomiser.deriveSeed("satellite");
    satellites.clear();
    satellites.resize(numSatellites);
}