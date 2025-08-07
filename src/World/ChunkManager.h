//
// Created by patrick on 8/4/25.
//

#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include "../Renderer/Renderer.h"
#include "NoiseGenerator.h"
#include "shader.h"

#include "Chunk.h"

struct GLMVec3Hash {
    size_t operator()(const glm::ivec3& v) const {
        size_t h1 = std::hash<int>()(v.x);
        size_t h2 = std::hash<int>()(v.y);
        size_t h3 = std::hash<int>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class ChunkManager {
private:
    static constexpr int WORLD_RADIUS = 5;
    static constexpr int WORLD_DEPTH = 1;
    NoiseGenerator m_noisegen;
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, GLMVec3Hash> m_chunks;
public:
    ChunkManager();
    void render(Renderer& renderer, Shader& shader) const;
    Chunk* getChunk(const glm::ivec3& chunkPos);
};



#endif //CHUNKMANAGER_H
