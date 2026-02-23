#include "Random.h"

#include <cstdlib>
#include <ctime>

Random::Random() {
    srand(static_cast<unsigned int>(std::time(nullptr)));
}

Random::Random(unsigned int seed) {
    srand(seed);
}

int Random::GetRandomInt(int minInclusive, int maxInclusive) {
    return rand() % (maxInclusive + 1) + minInclusive;
}

float Random::GetRandomFloat(float minInclusive, float maxInclusive) {
    float normalized = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return minInclusive + normalized * (maxInclusive - minInclusive);
}
