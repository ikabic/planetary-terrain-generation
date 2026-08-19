#pragma once

#include <random>

class Randomiser {
private:
    uint32_t seed = 24241;
    std::mt19937 generator;

public:
    Randomiser() : seed(seed), generator(seed) {}

	uint32_t getSeed() const { return seed; }

    void setSeed(uint32_t newSeed) { seed = newSeed; generator.seed(seed); }

    uint32_t deriveSeed(const std::string& name);

	float floatRange(float min, float max, float bias = 1.0f); // bias > 1.0f skews towards min, bias < 1.0f skews towards max
    int intRange(int min, int max);

	int weightedChoice(const std::vector<float>& weights);

    bool chance(float probability);
};

inline Randomiser randomiser;