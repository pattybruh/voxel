//
// Created by Patrick Li
//

#ifndef CHUNK_H
#define CHUNK_H
#include "Block.h"
#include <glm/glm.hpp>
#include "../Renderer/VertexArray.h"
#include "../Renderer/VertexBuffer.h"
#include "../Renderer/IndexBuffer.h"
#include <memory>

#include "NoiseGenerator.h"

struct Vertex{
    glm::vec3 pos;
    glm::vec3 norm;
    //glm::vec3 texCoord;
};

class Chunk{
private:
    std::unique_ptr<VertexArray> m_va;
    std::unique_ptr<VertexBuffer> m_vb;
    std::unique_ptr<IndexBuffer> m_ib;
    std::unique_ptr<VertexBufferLayout> m_lay;
	Block ***m_blocks;
    bool m_dirty = false;
    //mesh data
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
public:
	static constexpr int CHUNK_SIZE = 16;
    static constexpr int MAX_HEIGHT = 32;
	Chunk();
    Chunk(const glm::ivec3& chunk_pos, NoiseGenerator& noisegen);
	~Chunk();
	void update_mesh();
    const VertexArray& get_va() const {return *m_va;}
    const IndexBuffer& get_ib() const {return *m_ib;}

    void mark_dirty() {m_dirty = true;}
    void clear_dirty() {m_dirty = false;}
    bool is_dirty() const {return m_dirty;}
    void set_block(const glm::ivec3& block_coord, BlockType type);
    Block& get_block(const glm::ivec3& block_coord) const {
        return m_blocks[block_coord.x][block_coord.z][block_coord.y];
    }
};


#endif //CHUNK_H
