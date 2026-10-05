//
// Created by patrick on 8/9/25.
//

#include "Physics.h"
#include "InputService.h"

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

int Physics::add_agent(const glm::vec3 &position) {
    m_agents.emplace_back();
    PBody& body = m_agents.back();
    body.id = static_cast<int>(m_agents.size())-1;
    body.position = position;
    body.velocity = glm::vec3{0.0f};
    body.h_extents = glm::vec3{0.3f, 0.9f, 0.3f};
    body.type = BodyType::Dynamic;
    body.is_grounded = false;
    return body.id;
}

void Physics::move_agent(const InputAction& action, float dt) {
    PBody& body = m_agents[action.agentId];
    constexpr float MAX_ACCELERATION = 25.0f; // blocks/s^2 from input
    constexpr float MAX_SPEED = 10.0f;        // blocks/s from input

    glm::vec2 direction{1.0f, 1.0f};
    direction.x *= action.xDir;
    direction.y *= action.zDir;
    if(action.jump && body.is_grounded) {
        body.velocity.y = 5.0f;
        body.is_grounded = false;
    }
    float input_length = glm::length(direction);
    if (input_length > 1.0f) {
        direction /= input_length;
    }

    glm::vec2 velocity(body.velocity.x, body.velocity.z);
    velocity += direction * MAX_ACCELERATION * dt;

    float speed = glm::length(velocity);
    if (speed > MAX_SPEED) {
        velocity *= MAX_SPEED / speed;
    }
    body.velocity.x = velocity.x;
    body.velocity.z = velocity.y;
}

void Physics::step(ChunkManager &chunkman, float delta) {
    for(PBody& body : m_agents) {
        if(body.type != BodyType::Dynamic) continue;

        constexpr float GROUND_FRICTION = 16.0f;
        if (body.is_grounded) {
            glm::vec2 horizontal_velocity(body.velocity.x, body.velocity.z);
            float speed = glm::length(horizontal_velocity);
            if (speed > 0.0f) {
                horizontal_velocity *= glm::max(0.0f, speed - GROUND_FRICTION * delta) / speed;
                body.velocity.x = horizontal_velocity.x;
                body.velocity.z = horizontal_velocity.y;
            }
        }

        body.velocity.y -= GRAVITY * delta;
        glm::vec3 toward = body.position + (body.velocity*delta);

        sweep(chunkman, body, toward.x, 0);
        body.is_grounded = sweep(chunkman, body, toward.y, 1);
        sweep(chunkman, body, toward.z, 2);
    }
}

const PBody& Physics::get_agent(int agentId) const {
   return m_agents[agentId];
}
