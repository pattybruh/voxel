//
// Created by patrick on 8/9/25.
//

#include "Physics.h"

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
    for(int i=0; i<10; i++) {
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
    return (axis == 1) && (ds <= EPSILON);
}

bool Physics::aabb_overlap(ChunkManager &chunkman, const glm::vec3 &pos, const glm::vec3 &half_ext) {
    //at feet of entity
    constexpr float padding = 1e-5f;
    glm::vec3 vmin = (pos-glm::vec3(half_ext.x, 0.0f, half_ext.z))-glm::vec3(padding);
    glm::vec3 vmax = (pos+half_ext)+glm::vec3(padding);

    int xm = static_cast<int>(std::floor(vmax.x));
    int ym = static_cast<int>(std::floor(vmax.y));
    int zm = static_cast<int>(std::floor(vmax.z));
    for(int x=static_cast<int>(std::floor(vmin.x)); x<xm; x++) {
        for(int y=static_cast<int>(std::floor(vmin.y)); y<ym; y++) {
            for(int z=static_cast<int>(std::floor(vmin.z)); z<zm; z++) {
                if(chunkman.is_solid_w(glm::vec3{x,y,z})) return true;
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
}
