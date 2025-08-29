//
// Created by patrick on 8/25/25.
//

#ifndef VOXEL_MATHUTIL_H
#define VOXEL_MATHUTIL_H

#include <glm/glm.hpp>

inline glm::vec3 safe_normalize(const glm::vec3& v) {
    float l2 = glm::dot(v, v);
    if (l2 > 1e-12f) return v / std::sqrt(l2);
    return glm::vec3(0.0f);
}
inline glm::vec2 safe_normalize(const glm::vec2& v) {
    float s = glm::length(v);
    return (s > 1e-12f) ? v / s : glm::vec2(0);
}
inline int floor_div(int a, int b) {
    int q = a / b, r = a % b;
    if (r && ((r > 0) != (b > 0))) --q;
    return q;
}
inline int floor_mod(int a, int b) {
    int m = a - floor_div(a, b) * b;
    return m;
}

#endif //VOXEL_MATHUTIL_H