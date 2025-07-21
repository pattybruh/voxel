//
// Created by Patrick Li
//

#include "Block.h"

Block::

Block::Block() : m_active(true), m_block_type(BlockType_Ground){
}

Block::~Block() {
}

bool Block::is_active() const {
	return m_active;
}

void Block::set_active(bool active) {
	m_active = active;
}
