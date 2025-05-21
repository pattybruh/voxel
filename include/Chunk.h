//
// Created by Patrick Li
//

#ifndef CHUNK_H
#define CHUNK_H
#include <Block.h>

class Chunk{
private:
	Block ***m_blocks;
public:
	static const int CHUNK_SIZE = 16;
	Chunk();
	~Chunk();
	void Update();
	void Render();
	void CreateMesh();
};


#endif //CHUNK_H
