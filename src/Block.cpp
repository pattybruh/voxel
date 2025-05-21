//
// Created by Patrick Li
//

#include "Block.h"

Block::Block() {
}

Block::~Block() {
}

bool Block::isActive() {
	return m_active;
}

void Block::setActive(bool active) {
	m_active = active;
}
