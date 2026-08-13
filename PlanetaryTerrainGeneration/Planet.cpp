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
}

void Planet::draw() const {
    for (auto& face : faces)
        face.draw();
}

void Planet::randomiseTraits() {
	bool isAlien = randomiser.intRange(0, 100) < 5; // 5% chance to roll a completely random alien planet
	type = isAlien ? ALIEN : static_cast<PlanetType>(randomiser.intRange(0, 7));

	temperature = randomiser.floatRange(0.0f, 1.0f);
	palette = paletteManager.get(type, temperature);
}