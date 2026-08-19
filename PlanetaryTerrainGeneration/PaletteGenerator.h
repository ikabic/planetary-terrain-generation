#pragma once

#include "Palette.h"
#include "Randomiser.h"

struct PaletteParams {
	PaletteType type;
	int colourCount;
	float baseHue, baseSat, baseVal;

	PaletteParams() 
		: type(static_cast<PaletteType>(randomiser.intRange(1, 7))), colourCount(randomiser.intRange(5, 12)),
		  baseHue(randomiser.floatRange(0.0f, 360.0f)), baseSat(randomiser.floatRange(0.1f, 0.7f)), baseVal(randomiser.floatRange(0.4f, 0.9f)) {}

	PaletteParams(PaletteType type, int colourCount, float baseHue, float baseSat, float baseVal)
		: type(type), colourCount(colourCount), baseHue(baseHue), baseSat(baseSat), baseVal(baseVal) {}
};

glm::vec3 hsvToRgb(float h, float s, float v);
glm::vec3 rgbToHsv(float r, float g, float b);
glm::vec3 adjustHsv(glm::vec3 rgb, float hueDelta, float satDelta, float valDelta);

inline glm::vec3 getBaseColour() {
	return hsvToRgb(randomiser.floatRange(0.0f, 360.0f), randomiser.floatRange(0.1f, 0.7f), randomiser.floatRange(0.4f, 0.9f));
}

Palette generatePalette(const PlanetPaletteConfig& planetConfig, float temperature);