#ifndef VOXEL_FLAG_RENDERER_H
#define VOXEL_FLAG_RENDERER_H

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/vec3.hpp>
#include <memory>

class Renderer;
class Shader;

class FlagRenderer {
private:
    VertexArray m_vertex_array;
    std::unique_ptr<VertexBuffer> m_vertex_buffer;
    std::unique_ptr<IndexBuffer> m_index_buffer;

public:
    FlagRenderer();
    void draw(const Renderer& renderer, Shader& shader, const glm::vec3& position) const;
};

#endif // VOXEL_FLAG_RENDERER_H
