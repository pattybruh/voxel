//
// Created by Patrick Li
//

#ifndef BLOCK_H
#define BLOCK_H

enum BlockType{
	BlockType_Default = 0,
	BlockType_Grass,
	BlockType_Dirt,
	BlockType_Wood,
	BlockType_Water,
};

//keep blocks simple, less overhead
class Block{
//dont need to store pos, normals (deduced), color (blocktype)
private:
	bool m_active;
	BlockType m_blockType;
public:
	Block();
	~Block();
	bool isActive();
	void setActive(bool active);
};

#endif //BLOCK_H
