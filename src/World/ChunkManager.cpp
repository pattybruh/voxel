//
// Created by patrick on 8/4/25.
//

#include "ChunkManager.h"
//#include <glm/gtc/matrix_transform.hpp>

ChunkManager::ChunkManager() {
    constexpr int HALF_W = WORLD_SIZE / 2;
    for(int i = -HALF_W; i < HALF_W; i++) {
        for(int j = -HALF_W; j < HALF_W; j++) {
            for(int k = -HALF_W; k < HALF_W; k++) {
                m_chunks.emplace(glm::ivec3{i,j,k}, std::make_unique<Chunk>());
            }
        }
    }
}

/*
void ChunkManager::render(Renderer &renderer, Shader &shader) const {
    for(const auto& [pos, chunk] : m_chunks) {
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos * Chunk::CHUNK_SIZE));
        glm::mat4 model = glm::mat4(1.0f);

        shader.setMat4("model", model);
        //renderer.draw(chunk->get_va(), chunk->get_ib(), shader);
        chunk->render();
        break;
    }
}
*/

Chunk* ChunkManager::getChunk(const glm::ivec3 &chunkPos) {
}
