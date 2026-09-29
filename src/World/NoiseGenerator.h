//
// Created by patrick on 8/6/25.
//

#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H

#include <vector>

struct TerrainSettings {
    float amplitude; // Vertical variation from the base height, in blocks
    float frequency;
    unsigned int seed;  // 0 for random seed
    TerrainSettings(float amp, float freq, unsigned int s=0)
        : amplitude(amp), frequency(freq), seed(s) {}
};

class NoiseGenerator {
private:
    std::vector<int> m_permutation;
    const TerrainSettings m_settings;

    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y);
    float get_noise(float x, float y);
public:
    NoiseGenerator(TerrainSettings settings);
    float get_perlin(float x, float y, unsigned int octave);
};



#endif //NOISEGENERATOR_H
