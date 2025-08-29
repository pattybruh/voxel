//
// Created by patrick
//

#ifndef VOXEL_IWQUERIES_H
#define VOXEL_IWQUERIES_H

#include <glm/glm.hpp>

struct RayHit {
    double d = 0.0f;    //distance along ray
    bool hit = false;
};

struct IWQueries {
    virtual ~IWQueries() = default;
    virtual RayHit raycast(const glm::dvec3& origin, const glm::dvec3& dir_unit, double max_range) const = 0;
};
#endif //VOXEL_IWQUERIES_H