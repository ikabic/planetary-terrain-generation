#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "PaletteGenerator.h"
#include "Randomiser.h"

struct Satellite {
    float size;
    float orbitRadius, orbitSpeed;
    float inclination, phase; // orbit tilt and starting angle in radians
    float verticalOffset = 0.0f;
    glm::vec3 colour;

    Satellite() {
        size = randomiser.floatRange(0.05f, 0.1f);
        orbitRadius = size + randomiser.floatRange(1.35f, 1.85f);
        orbitSpeed = randomiser.floatRange(0.175f, 1.1f) * (randomiser.chance(0.5f) ? 1.0f : -1.0f);
        inclination = glm::radians(randomiser.floatRange(-40.0f, 40.0f));
        phase = randomiser.floatRange(0.0f, glm::two_pi<float>());
		colour = getBaseColour();
    }

	Satellite(float tilt, float innerRadius, float outerRadius, glm::vec3 baseColour) { // for shared parameter satellite groups (rings)
        size = randomiser.floatRange(0.01f, 0.04f);
        orbitRadius = randomiser.floatRange(innerRadius, outerRadius);
        orbitSpeed = (1.2f / std::sqrt(orbitRadius)) * 0.2f;
        inclination = tilt + glm::radians(randomiser.floatRange(-0.3f, 0.3f));
        phase = randomiser.floatRange(0.0f, glm::two_pi<float>());
        verticalOffset = randomiser.floatRange(-0.07f, 0.07f);
        colour = randomiser.chance(0.5) ? adjustHsv(baseColour, +45.0f, -0.20f, +0.15f) : adjustHsv(baseColour, +45.0f, +0.20f, -0.20f); // 50/50 chance to get light vs dark shade
    }
};