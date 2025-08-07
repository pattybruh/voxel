//
// Created by patrick on 8/6/25.
//

#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H

#include <vector>


class NoiseGenerator {
private:
    std::vector<int> m_permutation;
    const float m_amp;
    const float m_freq;

    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y);
    float get_noise(float x, float y);
public:
    NoiseGenerator(float amp, float freq, unsigned int seed=0);
    float get_perlin(float x, float y, unsigned int octave);
};



#endif //NOISEGENERATOR_H
