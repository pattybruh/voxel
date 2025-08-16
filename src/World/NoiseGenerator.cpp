//
// Created by patrick on 8/6/25.
//

#include "NoiseGenerator.h"

#include <algorithm>
#include <random>

float NoiseGenerator::fade(float t) {
    return ((6*t - 15)*t + 10)*t*t*t;
}

float NoiseGenerator::lerp(float t, float a, float b) {
    return a + t*(b-a);
}

float NoiseGenerator::grad(int hash, float x, float y) {
    int h = hash & 0x3F;
    float u = (h < 4) ? x : y;
    float v = (h < 4) ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

NoiseGenerator::NoiseGenerator(float amp, float freq, unsigned int seed)
    : m_permutation(512), m_amp(amp), m_freq(freq)
{
    std::random_device rd;
    std::mt19937 gen((seed==0) ? rd() : seed);
    for(int i=0; i<256; i++) {
        m_permutation[i]=i;
    }
    std::shuffle(m_permutation.begin(), m_permutation.begin()+256, gen);
    for(int i=0; i<256; i++) {
        m_permutation[i+256] = m_permutation[i];
    }
}

float NoiseGenerator::get_perlin(float x, float y, unsigned int octave) {
    float res = 0.0f;
    float amp = m_amp;
    float maxamp = 0.0f;
    float freq = m_freq;
    for(unsigned int o=0; o<octave; o++) {
        res += amp*get_noise(x*freq, y*freq);
        maxamp += amp;
        amp *= 0.5f;
        freq *= 2.0f;
    }
    return (res/maxamp + 1.0f)*0.5f;
}

float NoiseGenerator::get_noise(float x, float y) {
    int ix = static_cast<int>(std::floor(x)) & 255;
    int iy = static_cast<int>(std::floor(y)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);
    float u = fade(x);
    float v = fade(y);

    int ia = m_permutation[ix] + iy;
    int ib = m_permutation[ix+1] + iy;

    float grad00 = grad(m_permutation[ia], x, y);
    float grad10 = grad(m_permutation[ib], x - 1, y);
    float grad01 = grad(m_permutation[ia+1], x, y-1);
    float grad11 = grad(m_permutation[ib+1], x-1, y-1);

    return lerp(v, lerp(u, grad00, grad10), lerp(u, grad01, grad11));
}
