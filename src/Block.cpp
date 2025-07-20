//
// Created by Patrick Li
//

#include "Block.h"

Block::

Block::Block() {
}

Block::~Block() {
}

bool Block::isActive() const {
	return m_active;
}

void Block::setActive(bool active) {
	m_active = active;
}
