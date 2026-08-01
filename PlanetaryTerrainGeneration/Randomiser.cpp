#include "Randomiser.h"

Randomiser randomiser;

float Randomiser::floatRange(float min, float max) {
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}

int Randomiser::intRange(int min, int max) {
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

uint32_t Randomiser::deriveSeed(const std::string& name) {
	std::hash<std::string> hasher;

	uint32_t derivedSeed = randomiser.getSeed() ^ static_cast<uint32_t>(hasher(name));
	generator.seed(derivedSeed);

	return derivedSeed;
}