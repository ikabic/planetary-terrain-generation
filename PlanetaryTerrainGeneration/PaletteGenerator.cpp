#include "PaletteGenerator.h"
#include "Randomiser.h"

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

Palette generatePalette(int numColours, PaletteType type) {
	Palette palette;
	randomiser.deriveSeed("palette");

	float baseHue = randomiser.floatRange(0.0f, 360.0f), baseSaturation = randomiser.floatRange(0.25f, 0.6f), baseValue = randomiser.floatRange(0.5f, 0.9f);
	constexpr float hueJitterRange = 6.0f;
	float boundStep = 1.0f / numColours;

	auto wrapHue = [](float h) {
		float result = std::fmod(h, 360.0f);
		return (result < 0) ? result + 360.0f : result;
	};

	float maxSteps = (numColours > 1) ? static_cast<float>(numColours - 1) : 1.0f;

	for (int i = 0; i < numColours; i++) {
		float h = baseHue, s = baseSaturation, v = baseValue, upperBound;

		switch (type) {
		case MONOCHROMATIC: {
			h = baseHue;
			s = (numColours == 1) ? baseSaturation : baseSaturation * (0.5f + 0.5f * (i / maxSteps));
			v = (numColours == 1) ? baseValue : baseValue * (1.0f - 0.6f * (i / maxSteps));
			break;
		}
		case ANALOGOUS: {
			float spread = 40.0f;
			float offset = (numColours == 1) ? 0.0f : ((float)i / maxSteps - 0.5f) * spread;
			h = wrapHue(baseHue + offset);
			break;
		}
		case COMPLEMENTARY: {
			float side = (i % 2 == 0) ? 0.0f : 180.0f;
			int groupIdx = i / 2;
			h = wrapHue(baseHue + side + groupIdx * 10.0f);
			v = std::clamp(baseValue - (groupIdx * 0.10f), 0.2f, 1.0f);
			break;
		}
		case SPLIT_COMPLEMENTARY: {
			static const float offsets[] = { 0.0f, 150.0f, 210.0f };
			int node = i % 3;
			int cycle = i / 3;
			h = wrapHue(baseHue + offsets[node] + cycle * 8.0f);
			s = std::clamp(baseSaturation - (cycle * 0.12f), 0.1f, 1.0f);
			break;
		}
		case TRIADIC: {
			int node = i % 3;
			int cycle = i / 3;
			h = wrapHue(baseHue + node * 120.0f);
			v = std::clamp(baseValue - (cycle * 0.12f), 0.2f, 1.0f);
			break;
		}
		case TETRADIC: {
			static const float offsets[] = { 0.0f, 60.0f, 180.0f, 240.0f };
			int node = i % 4;
			int cycle = i / 4;
			h = wrapHue(baseHue + offsets[node]);
			s = std::clamp(baseSaturation - (cycle * 0.12f), 0.1f, 1.0f);
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