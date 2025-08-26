//
// Created by patrick on 8/9/25.
//

#ifndef VOXEL_PHYSICS_H
#define VOXEL_PHYSICS_H
#include <glm/glm.hpp>

#include "Entity_Types.h"

class ChunkManager;

class Physics {
private:
    bool sweep(ChunkManager& chunkman, PBody& body, float target, int axis);
    bool aabb_overlap(ChunkManager& chunkman, const glm::vec3& pos, const glm::vec3& half_ext);
    void apply_drag(PBody& body, float dt);
public:
    static constexpr float GRAVITY = 9.81f;
    static constexpr float EPSILON = 1e-5;
    static constexpr float FRICTION_GROUND = 10.0f;
    Physics();
    void step(ChunkManager& chunkman, PBody& body, float delta);
};


#endif //VOXEL_PHYSICS_H