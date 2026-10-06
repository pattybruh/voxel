//
// Created by patrick on 8/9/25.
//

#ifndef VOXEL_PHYSICS_H
#define VOXEL_PHYSICS_H
#include <glm/glm.hpp>

#include "../World/ChunkManager.h"
#include <GLFW/glfw3.h>

enum class BodyType : int {
    Static, Dynamic
};

struct PBody {
    int id;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 h_extents;
    BodyType type = BodyType::Dynamic;
    bool is_grounded = false;
};

struct InputAction{
    float xDir;
    float zDir;
    int agentId;
    bool jump;
};


class Physics {
private:
    std::vector<PBody> m_agents;

    bool sweep(ChunkManager& chunkman, PBody& body, float target, int axis);
    bool aabb_overlap(ChunkManager& chunkman, const glm::vec3& pos, const glm::vec3& half_ext);
public:
    static constexpr float GRAVITY = 9.81f;
    static constexpr float EPSILON = 1e-5;

    Physics();
    void reset();
    int add_agent(const glm::vec3& position);
    void move_agent(const InputAction& action, float dt);
    void step(ChunkManager& chunkman, float delta);
    const PBody& get_agent(int agentId) const;
};


#endif //VOXEL_PHYSICS_H
