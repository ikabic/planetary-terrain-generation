#pragma once

#include "Palette.h"
#include "Randomiser.h"

struct PaletteParams {
	PaletteType type;
	int numColours;
	float baseHue, baseSat, baseVal;

	PaletteParams() 
		: type(static_cast<PaletteType>(randomiser.intRange(1, 7))), numColours(randomiser.intRange(5, 12)),
		  baseHue(randomiser.floatRange(0.0f, 360.0f)), baseSat(randomiser.floatRange(0.1f, 0.7f)), baseVal(randomiser.floatRange(0.4f, 0.9f)) {}

	PaletteParams(PaletteType type, int numColours, float baseHue, float baseSat, float baseVal)
		: type(type), numColours(numColours), baseHue(baseHue), baseSat(baseSat), baseVal(baseVal) {}
};

glm::vec3 hsvToRgb(float h, float s, float v);

Palette generatePalette(const PlanetPaletteConfig& planetConfig, float temperature);