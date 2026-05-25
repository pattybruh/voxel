//
// Created by patrick on 8/9/25.
//

#include "Physics.h"
#include "InputService.h"

inline glm::vec3 safe_normalize(const glm::vec3& v) {
    float l2 = glm::dot(v, v);
    if (l2 > 1e-12f) return v / std::sqrt(l2);
    return glm::vec3(0.0f);
}

void move_player_horizontal(GLFWwindow* window, const Camera& cam, PBody& body, float ms) {
    glm::vec3 fwd = {cam.get_front().x, 0.0f, cam.get_front().z};
    auto len2 = glm::dot(fwd, fwd);
    if (len2 > 1e-12f) fwd /= std::sqrt(len2); else fwd = {0,0,1};
    //fwd = safe_normalize(fwd);
    glm::vec3 right = {-fwd.z, 0.0f, fwd.x};
    glm::vec3 direction(0);
    char input_key = '\0';
    if(GetInput(input_key)) {
        switch(input_key) {
        case 'w':
            direction += fwd;
            break;
        case 'a':
            direction -= right;
            break;
        case 's':
            direction -= fwd;
            break;
        case 'd':
            direction += right;
            break;
        case ' ':
            if(body.is_grounded) {
                body.velocity.y = 5.0f;
                body.is_grounded = false;
            }
            break;
        default:
            break;
        }
    }
    if (glm::dot(direction, direction) > 0.0f) {
        direction = safe_normalize(direction);
        body.velocity.x = glm::clamp(body.velocity.x+direction.x, -ms, ms);
        body.velocity.z = glm::clamp(body.velocity.z+direction.z, -ms, ms);
    }
}

void try_jump(GLFWwindow* window, PBody& body, float jumpSpeed) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && body.is_grounded) {
        body.velocity.y = jumpSpeed;
        body.is_grounded = false;
    }
}

bool Physics::sweep(ChunkManager &chunkman, PBody &body, float target, int axis) {
    float start = (&body.position.x)[axis];
    float ds = target-start;
    if(fabs(ds) < EPSILON) return false;

    glm::vec3 testpos = body.position;
    (&testpos.x)[axis] = target;

    if (!aabb_overlap(chunkman, testpos, body.h_extents)) {
        (&body.position.x)[axis] = target;
        return false;
    }

    float low = start, high = target;
    for(int i=0; i<12; i++) {
        float mid = 0.5f*(low+high);
        testpos = body.position;
        (&testpos.x)[axis] = mid;

        if (aabb_overlap(chunkman, testpos, body.h_extents)) {
            high = mid;
        } else {
            low = mid;
        }
    }
    (&body.position.x)[axis] = low;
    (&body.velocity.x)[axis] = 0.0f;
    return (axis == 1) && (ds < 0.0f);
}

bool Physics::aabb_overlap(ChunkManager &chunkman, const glm::vec3 &pos, const glm::vec3 &half_ext) {
    //at feet of entity
    constexpr float padding = 1e-5f;
    glm::vec3 vmin = (pos-glm::vec3(half_ext.x, 0.0f, half_ext.z))-glm::vec3(padding);
    glm::vec3 vmax = (pos+half_ext)+glm::vec3(padding);

    int xm = static_cast<int>(std::floor(vmax.x-padding));
    int ym = static_cast<int>(std::floor(vmax.y-padding));
    int zm = static_cast<int>(std::floor(vmax.z-padding));

    for(int x=static_cast<int>(std::floor(vmin.x)); x<=xm; x++) {
        for(int y=static_cast<int>(std::floor(vmin.y)); y<=ym; y++) {
            for(int z=static_cast<int>(std::floor(vmin.z)); z<=zm; z++) {
                if(chunkman.is_solid_w(glm::ivec3{x,y,z})) return true;
            }
        }
    }
    return false;
}

Physics::Physics() {
}

void Physics::step(ChunkManager &chunkman, PBody &body, float delta) {
    if(body.type != BodyType::Dynamic) return;

    body.velocity.y -= GRAVITY * delta;
    glm::vec3 toward = body.position + (body.velocity*delta);

    sweep(chunkman, body, toward.x, 0);
    body.is_grounded = sweep(chunkman, body, toward.y, 1);
    sweep(chunkman, body, toward.z, 2);
    if(body.is_grounded){
        body.velocity.x *= 0.8;
        body.velocity.z *= 0.8;
    }
}
