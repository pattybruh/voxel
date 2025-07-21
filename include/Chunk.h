//
// Created by Patrick Li
//

#ifndef CHUNK_H
#define CHUNK_H
#include "Block.h"
#include <glm/glm.hpp>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <memory>

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
    //mesh data
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
public:
	static const int CHUNK_SIZE = 16;
	Chunk();
	~Chunk();
	void create_mesh();
	void update();
	void render() const;
};


#endif //CHUNK_H
