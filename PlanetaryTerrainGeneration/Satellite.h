#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "PaletteGenerator.h"

struct Satellite {
    float size;
    float orbitRadius, orbitSpeed;
    float inclination, phase; // orbit tilt and starting angle in radians
    glm::vec3 colour;

    Satellite() {
        size = randomiser.floatRange(0.05f, 0.1f);
        orbitRadius = size + randomiser.floatRange(1.35f, 1.85f);
        orbitSpeed = randomiser.floatRange(0.175f, 1.1f) * (randomiser.chance(0.5f) ? 1.0f : -1.0f);
        inclination = glm::radians(randomiser.floatRange(-40.0f, 40.0f));
        phase = randomiser.floatRange(0.0f, glm::two_pi<float>());
		colour = getBaseColour();
    }
};