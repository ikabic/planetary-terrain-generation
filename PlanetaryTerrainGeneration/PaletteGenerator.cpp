#include "PaletteGenerator.h"
#include "Planet.h"

glm::vec3 hsvToRgb(float h, float s, float v) {
	h = std::fmod(h, 360.0f);
	if (h < 0.0f) h += 360.0f;

	float c = v * s;
	float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
	float m = v - c;

	float r, g, b;

	if (h < 60.0f) { r = c; g = x; b = 0.0f; }
	else if (h < 120.0f) { r = x; g = c; b = 0.0f; }
	else if (h < 180.0f) { r = 0.0f; g = c; b = x; }
	else if (h < 240.0f) { r = 0.0f; g = x; b = c; }
	else if (h < 300.0f) { r = x; g = 0.0f; b = c; }
	else { r = c; g = 0.0f; b = x; }

	return glm::vec3(r + m, g + m, b + m);
}

static PaletteParams generatePaletteParams(const PlanetPaletteConfig& planetConfig, float temperature) {
	if (planetConfig.palettes.empty()) return PaletteParams();

	randomiser.deriveSeed("palette");

	std::vector<float> weights;
	for (const auto& p : planetConfig.palettes) weights.push_back(p.weight);

	int chosenIndex = randomiser.weightedChoice(weights);
	const PaletteConfig& selectedPalette = planetConfig.palettes[chosenIndex];

	if (!selectedPalette.biomeStops.empty()) return PaletteParams(CUSTOM, 0, 0.0f, 0.0f, 0.0f);

	glm::vec2 hRange, sRange, vRange;
	selectedPalette.getHSVBounds(temperature, hRange, sRange, vRange);

	float baseHue = randomiser.floatRange(hRange.x, hRange.y);
	float baseSat = randomiser.floatRange(sRange.x, sRange.y);
	float baseVal = randomiser.floatRange(vRange.x, vRange.y);

	int numColours = randomiser.intRange(selectedPalette.minColours, selectedPalette.maxColours);

	return PaletteParams(selectedPalette.type, numColours, baseHue, baseSat, baseVal);
}

static Palette constructCustomPalette(const PaletteConfig& selectedPalette) {
	Palette palette;

	for (const auto& stop : selectedPalette.biomeStops) {
		float h = randomiser.floatRange(stop.h.x, stop.h.y);
		float s = randomiser.floatRange(stop.s.x, stop.s.y);
		float v = randomiser.floatRange(stop.v.x, stop.v.y);

		palette.colours.push_back(hsvToRgb(h, s, v));
		palette.upperBounds.push_back(stop.upperBound);
	}
	return palette;
}

Palette generateSelectedPalette(PaletteParams paletteParams) {
	Palette palette;

	constexpr float hueJitterRange = 6.0f;
	float boundStep = 1.0f / paletteParams.numColours;

	auto wrapHue = [](float h) {
		float result = std::fmod(h, 360.0f);
		return (result < 0) ? result + 360.0f : result;
	};

	float maxSteps = (paletteParams.numColours > 1) ? static_cast<float>(paletteParams.numColours - 1) : 1.0f;
	float hueSpread = randomiser.floatRange(15.0f, 30.0f);

	for (int i = 0; i < paletteParams.numColours; i++) {
		float h = paletteParams.baseHue, s = paletteParams.baseSat, v = paletteParams.baseVal, upperBound;

		switch (paletteParams.type) {
		case MONOCHROMATIC: {
			h = paletteParams.baseHue;
			s = (paletteParams.numColours == 1) ? paletteParams.baseSat : paletteParams.baseSat * (0.5f + 0.5f * (i / maxSteps));
			v = (paletteParams.numColours == 1) ? paletteParams.baseVal : paletteParams.baseVal * (1.0f - 0.4f * (i / maxSteps));
			break;
		}
		case ANALOGOUS: {
			float offset = (paletteParams.numColours == 1) ? 0.0f : ((float)i / maxSteps - 0.5f) * hueSpread;
			h = wrapHue(paletteParams.baseHue + offset);
			break;
		}
		case COMPLEMENTARY: {
			float side = (i % 2 == 0) ? 0.0f : 180.0f;
			int groupIdx = i / 2;
			h = wrapHue(paletteParams.baseHue + side + groupIdx * 10.0f);
			v = std::clamp(paletteParams.baseVal - (groupIdx * 0.10f), 0.2f, 1.0f);
			break;
		}
		case SPLIT_COMPLEMENTARY: {
			static const float offsets[] = { 0.0f, 150.0f, 210.0f };
			int node = i % 3;
			int cycle = i / 3;
			h = wrapHue(paletteParams.baseHue + offsets[node] + cycle * 8.0f);
			s = std::clamp(paletteParams.baseSat - (cycle * 0.12f), 0.0f, 0.7f);
			break;
		}
		case TRIADIC: {
			int node = i % 3;
			int cycle = i / 3;
			h = wrapHue(paletteParams.baseHue + node * 120.0f);
			v = std::clamp(paletteParams.baseVal - (cycle * 0.12f), 0.3f, 0.9f);
			break;
		}
		case TETRADIC: {
			static const float offsets[] = { 0.0f, 60.0f, 180.0f, 240.0f };
			int node = i % 4;
			int cycle = i / 4;
			h = wrapHue(paletteParams.baseHue + offsets[node]);
			s = std::clamp(paletteParams.baseSat - (cycle * 0.12f), 0.0f, 0.7f);
			break;
		}
		case THERMAL: {
			float t = (paletteParams.numColours > 1) ? 1.0f - (static_cast<float>(i) / maxSteps) : 1.0f; // 1 is hottest, 0 is coldest

			constexpr float thermalHueSweep = 60.0f; // total hue travel from the coldest (start) to the hottest point
			h = wrapHue(paletteParams.baseHue + thermalHueSweep * std::pow(t, 1.8f));

			float sCurve = (t < 0.5f) ? (0.70f + 0.50f * t) : (1.0f - std::pow((t - 0.5f) / 0.5f, 2.5f));
			s = std::clamp(paletteParams.baseSat * sCurve, 0.0f, 1.0f);

			v = std::clamp(paletteParams.baseVal * std::pow(t, 0.5f), 0.03f, 1.0f);
			break;
		}
		default: return Palette();
		}

		upperBound = randomiser.floatRange(i * boundStep, (i + 1) * boundStep);

		h = wrapHue(h + randomiser.floatRange(-hueJitterRange, hueJitterRange));
		palette.colours.push_back(hsvToRgb(h, s, v));
		palette.upperBounds.push_back(upperBound);
	}

	std::sort(palette.upperBounds.begin(), palette.upperBounds.end());
	return palette;
}

Palette generatePalette(const PlanetPaletteConfig& planetConfig, float temperature) {
	PaletteParams paletteParams(generatePaletteParams(planetConfig, temperature));

	return paletteParams.type == CUSTOM 
		? constructCustomPalette(planetConfig.palettes[0])
		: generateSelectedPalette(paletteParams);
}