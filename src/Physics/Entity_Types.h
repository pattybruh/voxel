//
// Created by patrick on 8/25/25.
//

#ifndef VOXEL_ENTITY_TYPES_H
#define VOXEL_ENTITY_TYPES_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

enum class BodyType : int {
    Static, Dynamic
};

struct PBody {
    int id;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 h_extents;
    glm::vec2 intent;
    BodyType type = BodyType::Dynamic;
    bool is_grounded = false;
    bool has_intent = false;
};

#endif //VOXEL_ENTITY_TYPES_H