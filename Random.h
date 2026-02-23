#pragma once

class Random {
public:
    Random();
    Random(unsigned int seed);

    int GetRandomInt(int minInclusive, int maxInclusive);
    float GetRandomFloat(float minInclusive, float maxInclusive);
};


