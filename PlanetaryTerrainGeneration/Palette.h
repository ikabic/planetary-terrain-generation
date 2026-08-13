#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>

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

NLOHMANN_JSON_SERIALIZE_ENUM(PaletteType, {
	{CUSTOM, "CUSTOM"},
	{MONOCHROMATIC, "MONOCHROMATIC"},
	{ANALOGOUS, "ANALOGOUS"},
	{COMPLEMENTARY, "COMPLEMENTARY"},
	{SPLIT_COMPLEMENTARY, "SPLIT_COMPLEMENTARY"},
	{TRIADIC, "TRIADIC"},
	{TETRADIC, "TETRADIC"},
	{THERMAL, "THERMAL"}
});

struct BiomeStop {
    float upperBound;
    glm::vec2 h, s, v;
};

struct HSVStop {
    float t;
    glm::vec2 h, s, v;
};

struct PaletteConfig {
    PaletteType type;
    float weight;
    int minColours, maxColours;
    std::vector<BiomeStop> biomeStops;
    std::vector<HSVStop> baseHSV; // sorted by t

    void getHSVBounds(float targetTemp, glm::vec2& outH, glm::vec2& outS, glm::vec2& outV) const {
        targetTemp = glm::clamp(targetTemp, 0.0f, 1.0f);

        if (baseHSV.empty()) {
            outH = { 0.0f, 360.0f }; outS = { 0.1f, 0.7f }; outV = { 0.4f, 0.9f };
            return;
        }

        // Out-of-bounds cases
        if (targetTemp <= baseHSV.front().t) {
            outH = baseHSV.front().h; outS = baseHSV.front().s; outV = baseHSV.front().v;
            return;
        }
        if (targetTemp >= baseHSV.back().t) {
            outH = baseHSV.back().h; outS = baseHSV.back().s; outV = baseHSV.back().v;
            return;
        }

        // Lerp between bounding stops
        for (int i = 0; i < baseHSV.size() - 1; ++i) {
            const auto& stop0 = baseHSV[i];
            const auto& stop1 = baseHSV[i + 1];

            if (targetTemp >= stop0.t && targetTemp <= stop1.t) {
                float u = (targetTemp - stop0.t) / (stop1.t - stop0.t);
                outH = glm::mix(stop0.h, stop1.h, u);
                outS = glm::mix(stop0.s, stop1.s, u);
                outV = glm::mix(stop0.v, stop1.v, u);
                return;
            }
        }
    }
};

struct PlanetPaletteConfig {
    int id;
    std::vector<PaletteConfig> palettes;
};

struct Palette {
	std::vector<glm::vec3> colours;
	std::vector<float> upperBounds;

	Palette() = default;
	Palette(std::vector<glm::vec3> colours, std::vector<float> upperBounds) : colours(colours), upperBounds(upperBounds) {}
};
