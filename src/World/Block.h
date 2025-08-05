//
// Created by Patrick Li
//

#ifndef BLOCK_H
#define BLOCK_H

enum BlockType{
	BlockType_Ground = 0,
    BlockType_Air,
	BlockType_Water,
};

//keep blocks simple, less overhead
//dont need to store pos, normals (deduced), color (blocktype)
class Block{
private:
    bool m_active;
	BlockType m_block_type;
public:
    static constexpr float CUBE_VERTICIES[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,

        0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f
    };
    static constexpr unsigned int CUBE_INDICES[] = {
        0, 1, 3,
        1, 2, 3,
        3, 2, 7,
        2, 6, 7,
        7, 6, 4,
        6, 5, 4,
        4, 5, 0,
        5, 1, 0,
        0, 3, 4,
        3, 7, 4,
        1, 5, 2,
        5, 6, 2
    };
    static constexpr float BLOCK_SIZE = 1.0f;
    Block();
	~Block();
	bool is_active() const;
	void set_active(bool active);
};

#endif //BLOCK_H
