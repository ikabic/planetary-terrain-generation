#include "Planet.h"
#include "PaletteManager.h"

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
    for (int i = 0; i < 6; i++)
        faces[i].build(directions[i]);

	randomiseTraits();
}

void Planet::draw() const {
    for (auto& face : faces)
        face.draw();
}

void Planet::randomiseTraits() {
	//palette = paletteManager.get("earth");
	palette = paletteManager.get();

}