//
//
#include "Chunk.h"

Chunk::Chunk() {
	m_blocks = new Block**[CHUNK_SIZE];
	for(int i=0; i<CHUNK_SIZE; i++) {
		m_blocks[i] = new Block*[CHUNK_SIZE];
		for(int j=0; j<CHUNK_SIZE; j++) {
			m_blocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
}

Chunk::~Chunk() {
	for(int i=0; i<CHUNK_SIZE; i++) {
		for(int j=0; j<CHUNK_SIZE; j++) {
			delete[] m_blocks[i][j];
		}
		delete[] m_blocks[i];
	}
	delete[] m_blocks;
}

void Chunk::create_mesh() {
    m_va = std::make_unique<VertexArray>();
    m_va->bind();
    m_vb = std::make_unique<VertexBuffer>(Block::CUBE_VERTICIES, sizeof(Block::CUBE_VERTICIES));
    m_lay = std::make_unique<VertexBufferLayout>();
    m_lay->push<float>(3);//pos
    m_lay->push<float>(3);//color
    m_va->add_buffer(*m_vb, *m_lay);
    m_ib = std::make_unique<IndexBuffer>(Block::CUBE_INDICES, 3*12);

    /*
    m_va->unbind();
    m_vb->unbind();
    m_ib->unbind();
    */
}
void Chunk::update() {
}

void Chunk::render() {
}
