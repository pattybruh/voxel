#include "FlagRenderer.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

// Local origin is the center of the pyramid's base on the terrain surface.
constexpr float VERTICES[] = {
    // position             color
    -0.6f, 0.02f, -0.6f,    1.0f, 0.8f, 0.1f,
     0.6f, 0.02f, -0.6f,    1.0f, 0.8f, 0.1f,
     0.6f, 0.02f,  0.6f,    1.0f, 0.8f, 0.1f,
    -0.6f, 0.02f,  0.6f,    1.0f, 0.8f, 0.1f,
     0.0f, 2.02f,  0.0f,    1.0f, 0.1f, 0.1f,
};

constexpr unsigned int INDICES[] = {
    0, 4, 1,
    1, 4, 2,
    2, 4, 3,
    3, 4, 0,
    0, 1, 2,
    0, 2, 3,
};

} // namespace

FlagRenderer::FlagRenderer() {
    VertexBufferLayout layout;
    layout.push<float>(3);  // position
    layout.push<float>(3);  // color

    m_vertex_array.bind();
    m_vertex_buffer = std::make_unique<VertexBuffer>(VERTICES, sizeof(VERTICES));
    m_vertex_array.add_buffer(*m_vertex_buffer, layout);
    m_index_buffer = std::make_unique<IndexBuffer>(INDICES, sizeof(INDICES) / sizeof(INDICES[0]));
    m_vertex_array.unbind();
    m_vertex_buffer->unbind();
}

void FlagRenderer::draw(const Renderer& renderer, Shader& shader, const glm::vec3& position) const {
    shader.use();
    shader.setMat4("model", glm::translate(glm::mat4{1.0f}, position));
    renderer.draw(m_vertex_array, *m_index_buffer, shader);
}
