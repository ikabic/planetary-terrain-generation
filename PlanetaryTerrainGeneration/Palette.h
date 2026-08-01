#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>

struct Palette {
	std::vector<glm::vec3> colours;
	std::vector<float> upperBounds;

	Palette() = default;
	Palette(std::vector<glm::vec3> colours, std::vector<float> upperBounds) : colours(colours), upperBounds(upperBounds) {}
};
