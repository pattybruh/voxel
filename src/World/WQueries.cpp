//
// Created by patrick
//

#include "WQueries.h"
#include "../Utils/MathUtil.h"
#include "ChunkManager.h"
#include <limits>

inline double WQueries::first_t(double o, int iw, int s, double dir) const {
    if(s==0) {
        return std::numeric_limits<double>::infinity();
    }
    double bound = (s > 0) ? (iw+1)*M_VOX : iw*M_VOX;
    return (bound-o)/dir;
}

RayHit WQueries::raycast(const glm::dvec3 &origin, const glm::dvec3 &dir_unit_n, double max_range) const {
    RayHit res;
    int iwx = static_cast<int>(std::floor(origin.x/M_VOX));
    int iwy = static_cast<int>(std::floor(origin.y/M_VOX));
    int iwz = static_cast<int>(std::floor(origin.z/M_VOX));
    if(m_chunkman->is_solid_w({iwx, iwy, iwx})) {
        res.d = 0.0f;
        res.hit = true;
        return res;
    }

    int sx = (dir_unit_n.x > 0) ? 1 : (dir_unit_n.x < 0 ? -1 : 0);
    int sy = (dir_unit_n.y > 0) ? 1 : (dir_unit_n.y < 0 ? -1 : 0);
    int sz = (dir_unit_n.z > 0) ? 1 : (dir_unit_n.z < 0 ? -1 : 0);
    double tmaxX = first_t(origin.x, iwx, sx, dir_unit_n.x);
    double tmaxY = first_t(origin.y, iwy, sy, dir_unit_n.y);
    double tmaxZ = first_t(origin.z, iwz, sz, dir_unit_n.z);
    double t = 0.0f;
    while(t <= max_range) {

        if(m_chunkman->is_solid_w({iwx, iwy, iwx})) {
            res.d = 0.0f;
            res.hit = true;
            return res;
        }
    }
    return res;
}
