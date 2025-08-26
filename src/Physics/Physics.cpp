//
// Created by patrick on 8/9/25.
//

#include "Physics.h"
#include "../World/ChunkManager.h"
#include "../Utils/MathUtil.h"

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

void Physics::apply_drag(PBody &body, float dt) {
    glm::vec2 v = {body.velocity.x, body.velocity.z};
    if(body.has_intent) {
        glm::vec2 u = safe_normalize(body.intent);
        float s_parv = glm::dot(v, u);
        glm::vec2 parv = s_parv*u;
        glm::vec2 latv = v-parv;
        if(body.is_grounded) {
            float target = 5.0f;
            float dv     = target-s_parv;
            float step   = 30.0f * dt;//TODO: pass in thru PBody or Physics.h constexpr
            float s_parv_new = (std::abs(dv) <= step) ? target : (s_parv + step*((dv > 0) ? 1.f : -1.f));

            v = (s_parv_new*u) + (latv*std::max(0.f, 1.f-(FRICTION_GROUND*dt)));
        }
        else {

        }
    }
    else {
        float scale = std::max(0.0f, 1.0f-(FRICTION_GROUND*dt));
        v *= scale;
        if(glm::length(v) < EPSILON) {
            v = glm::vec2(0);
        }
    }
    body.velocity.x = v.x;
    body.velocity.z = v.y;
}


void Physics::step(ChunkManager &chunkman, PBody &body, float delta) {
    if(body.type != BodyType::Dynamic) return;

    apply_drag(body, delta);

    body.velocity.y -= GRAVITY * delta;
    glm::vec3 toward = body.position + (body.velocity*delta);

    sweep(chunkman, body, toward.x, 0);
    body.is_grounded = sweep(chunkman, body, toward.y, 1);
    sweep(chunkman, body, toward.z, 2);
    //TODO: frction only if on ground with no intent to move
    if(body.is_grounded && !body.has_intent){
        body.velocity.x *= 0.85;
        body.velocity.z *= 0.85;
    }
}
