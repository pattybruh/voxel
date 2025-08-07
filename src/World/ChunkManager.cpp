//
// Created by patrick on 8/4/25.
//

#include "ChunkManager.h"
#include <glm/gtc/matrix_transform.hpp>

ChunkManager::ChunkManager() : m_noisegen(3.0f, 0.01f){
    constexpr int world_d = WORLD_DEPTH/2;
    for(int x = -WORLD_RADIUS; x < WORLD_RADIUS; x++) {
        for(int y = -WORLD_DEPTH; y < WORLD_DEPTH+2; y++) {
            for(int z = -WORLD_RADIUS; z < WORLD_RADIUS; z++) {
                m_chunks.emplace(glm::ivec3{x,y,z}, std::make_unique<Chunk>(glm::ivec3{x,y,z}, m_noisegen));
            }
        }
    }
}

void ChunkManager::render(Renderer &renderer, Shader &shader) const {
    for(const auto& [pos, chunk] : m_chunks) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos * Chunk::CHUNK_SIZE));

        shader.setMat4("model", model);
        renderer.draw(chunk->get_va(), chunk->get_ib(), shader);
    }
}

Chunk* ChunkManager::getChunk(const glm::ivec3 &chunkPos) {
}
