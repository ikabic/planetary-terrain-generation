#pragma once

#include "Palette.h"

enum PaletteType {
	CUSTOM,
	MONOCHROMATIC,
	ANALOGOUS,
	COMPLEMENTARY,
	SPLIT_COMPLEMENTARY,
	TRIADIC,
	TETRADIC,
	THERMAL
};

glm::vec3 hsvToRgb(float h, float s, float v);

Palette generatePalette(int numColours, PaletteType type);