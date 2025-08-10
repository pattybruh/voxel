//
// Created by patrick on 8/9/25.
//

#ifndef VOXEL_PHYSICS_H
#define VOXEL_PHYSICS_H
#include <glm/glm.hpp>

#include "../World/ChunkManager.h"
#include "../Renderer/Camera.h"

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

inline glm::vec3 safe_normalize(const glm::vec3& v) {
    float l2 = glm::dot(v, v);
    if (l2 > 1e-12f) return v / std::sqrt(l2);
    return glm::vec3(0.0f);
}
inline void move_player_horizontal(GLFWwindow* window, const Camera& cam, PBody& body, float ms) {
    glm::vec3 fwd = {cam.get_front().x, 0.0f, cam.get_front().z};
    fwd = safe_normalize(fwd);
    glm::vec3 right = {-fwd.z, 0.0f, fwd.x};
    if (fwd == glm::vec3(0) && right == glm::vec3(0)) {
        fwd = {0.0f, 0.0f, 1.0f};
        right = {-1.0f, 0.0f, 0.0f};
    }
    glm::vec3 direction(0);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        direction += fwd;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        direction -= right;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        direction -= fwd;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        direction += right;
    }
    if (glm::dot(direction, direction) > 0.0f) direction = safe_normalize(direction);
    body.velocity.x += direction.x;
    body.velocity.z += direction.z;
}
inline void try_jump(GLFWwindow* window, PBody& body, float jumpSpeed=5.5f) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && body.is_grounded) {
        body.velocity.y = jumpSpeed;
        body.is_grounded = false;
    }
}

class Physics {
private:
    bool sweep(ChunkManager& chunkman, PBody& body, float target, int axis);
    bool aabb_overlap(ChunkManager& chunkman, const glm::vec3& pos, const glm::vec3& half_ext);
public:
    static constexpr float GRAVITY = 9.81f;
    static constexpr float EPSILON = 1e-5;
    Physics();
    void step(ChunkManager& chunkman, PBody& body, float delta);
};


#endif //VOXEL_PHYSICS_H