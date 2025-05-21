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

void Chunk::Update() {
}

void Chunk::Render() {
}

void Chunk::CreateMesh() {

}
