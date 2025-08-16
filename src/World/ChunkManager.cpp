//
// Created by patrick on 8/4/25.
//

#include "ChunkManager.h"
#include <glm/gtc/matrix_transform.hpp>

inline int floor_div(int a, int b) {
    int q = a / b, r = a % b;
    if (r && ((r > 0) != (b > 0))) --q;
    return q;
}
inline int floor_mod(int a, int b) {
    int m = a - floor_div(a, b) * b;
    return m;
}

ChunkManager::ChunkManager() : m_noisegen(6.0f, 0.01f){
    for(int x = -WORLD_RADIUS; x < WORLD_RADIUS; x++) {
        for(int y = -WORLD_DEPTH; y < WORLD_DEPTH+3; y++) {
            for(int z = -WORLD_RADIUS; z < WORLD_RADIUS; z++) {
                m_chunks.emplace(glm::ivec3{x,y,z}, std::make_unique<Chunk>(glm::ivec3{x,y,z}, m_noisegen));
            }
        }
    }
}

void ChunkManager::mark_dirty_chunk(Chunk *dchunk) {
    if(dchunk) {
        dchunk->mark_dirty();
        m_dirty_chunks.insert(dchunk);
    }
}

void ChunkManager::update_dirty_chunks() {
    for(Chunk* dchunk : m_dirty_chunks) {
        dchunk->update_mesh();
        dchunk->clear_dirty();
    }
    m_dirty_chunks.clear();
}

void ChunkManager::render(Renderer &renderer, Shader &shader) const {
    for(const auto& [pos, chunk] : m_chunks) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos * Chunk::CHUNK_SIZE));

        shader.setMat4("model", model);
        renderer.draw(chunk->get_va(), chunk->get_ib(), shader);
    }
}

Chunk* ChunkManager::get_chunk(const glm::ivec3 &c_pos) {
    auto it = m_chunks.find(c_pos);
    return (it!=m_chunks.end()) ? it->second.get() : nullptr;
}

const Chunk * ChunkManager::get_chunk(const glm::ivec3 &c_pos) const {
    auto it = m_chunks.find(c_pos);
    return (it!=m_chunks.end()) ? it->second.get() : nullptr;
}

Chunk * ChunkManager::get_chunk_containing(const glm::ivec3 &w_pos) {
    return get_chunk(glm::vec3{
        floor_div(w_pos.x, Chunk::CHUNK_SIZE),
        floor_div(w_pos.y, Chunk::CHUNK_SIZE),
        floor_div(w_pos.z, Chunk::CHUNK_SIZE)
    });
}

const Chunk * ChunkManager::get_chunk_containing(const glm::ivec3 &w_pos) const {
    return get_chunk(glm::vec3{
        floor_div(w_pos.x, Chunk::CHUNK_SIZE),
        floor_div(w_pos.y, Chunk::CHUNK_SIZE),
        floor_div(w_pos.z, Chunk::CHUNK_SIZE)
    });
}

bool ChunkManager::is_solid_w(const glm::ivec3 &w_pos) {
    const Chunk* chunk = get_chunk_containing(w_pos);
    if(!chunk) return false;

    const glm::ivec3 block_coord{
        floor_mod(w_pos.x, Chunk::CHUNK_SIZE),
        floor_mod(w_pos.y, Chunk::CHUNK_SIZE),
        floor_mod(w_pos.z, Chunk::CHUNK_SIZE)};
    const Block& b = chunk->get_block(block_coord);
    return b.get_type() == BlockType::Ground;
}

void ChunkManager::set_block(const glm::ivec3& world_coord, BlockType type) {
    Chunk* chunk = get_chunk(world_coord);
    if(!chunk) return;

    const glm::ivec3 block_coord{world_coord.x%Chunk::CHUNK_SIZE, world_coord.y%Chunk::CHUNK_SIZE, world_coord.z%Chunk::CHUNK_SIZE};
    chunk->set_block(block_coord, type);
    mark_dirty_chunk(chunk);
    if(block_coord.x == 0) {
        mark_dirty_chunk(get_chunk(glm::ivec3{world_coord.x-1, world_coord.y, world_coord.z}));
    }
    else if(block_coord.x == Chunk::CHUNK_SIZE-1) {
        mark_dirty_chunk(get_chunk(glm::ivec3{world_coord.x+1, world_coord.y, world_coord.z}));
    }
    if(block_coord.y == 0) {
        mark_dirty_chunk(get_chunk(glm::ivec3{world_coord.x, world_coord.y-1, world_coord.z}));
    }
    else if(block_coord.y == Chunk::CHUNK_SIZE-1) {
        mark_dirty_chunk(get_chunk(glm::ivec3{world_coord.x, world_coord.y+1, world_coord.z}));
    }
    if(block_coord.z == 0) {
        mark_dirty_chunk(get_chunk(glm::ivec3{world_coord.x, world_coord.y, world_coord.z-1}));
    }
    else if(block_coord.z == Chunk::CHUNK_SIZE-1) {
        mark_dirty_chunk(get_chunk(glm::ivec3{world_coord.x, world_coord.y, world_coord.z+1}));
    }
}
