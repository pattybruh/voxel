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
    delete m_va;
    delete m_vb;
    delete m_ib;
}

    VertexArray* va = new VertexArray();
    va->bind();
    VertexBuffer* vb = new VertexBuffer(cubeVertices, sizeof(cubeVertices));
    VertexBufferLayout* lay = new VertexBufferLayout();
    lay->push<float>(3);//pos
    lay->push<float>(3);//color
    va->addBuffer(*vb, *lay);
    IndexBuffer* ib = new IndexBuffer(cubeIndices, 3*12);

    va->unbind();
    vb->unbind();
    ib->unbind();
void Chunk::createMesh() {

}
void Chunk::update() {
}

void Chunk::render() {
}

