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
    create_mesh();
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
    std::vector<std::array<float, 3>> mesh_vertices;
    std::vector<unsigned int> mesh_indices;
    unsigned int idx = 0;
    for(int x=0; x<CHUNK_SIZE; x++) {
        for(int y=0; y<CHUNK_SIZE; y++) {
            for(int z=0; z<CHUNK_SIZE; z++) {
                if(!m_blocks[x][y][z].is_active()) continue;

                if(x == 0 || !m_blocks[x-1][y][z].is_active()) {
                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 0.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 0.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 1.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 1.0f, 0.0f});

                    mesh_indices.insert(mesh_indices.end(), {idx, idx+1, idx+2, idx+2, idx+3, idx});
                    idx += 4;
                }
                if(z == 0 || !m_blocks[x][y][z-1].is_active()) {
                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 0.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 0.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 1.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 1.0f, 0.0f});

                    mesh_indices.insert(mesh_indices.end(), {idx, idx+1, idx+2, idx+2, idx+3, idx});
                    idx += 4;
                }
                if(x == CHUNK_SIZE-1 || !m_blocks[x+1][y][z].is_active()) {
                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 0.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 0.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 1.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 1.0f, 1.0f});

                    mesh_indices.insert(mesh_indices.end(), {idx, idx+1, idx+2, idx+2, idx+3, idx});
                    idx += 4;
                }
                if(z == CHUNK_SIZE-1 || !m_blocks[x][y][z+1].is_active()) {
                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 0.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 0.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 1.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 1.0f, 1.0f});

                    mesh_indices.insert(mesh_indices.end(), {idx, idx+1, idx+2, idx+2, idx+3, idx});
                    idx += 4;
                }
                if(y == 0 || !m_blocks[x][y-1][z].is_active()) {
                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 0.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 0.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 0.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, y*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 0.0f, 0.0f});

                    mesh_indices.insert(mesh_indices.end(), {idx, idx+1, idx+2, idx+2, idx+3, idx});
                    idx += 4;
                }
                if(y == CHUNK_SIZE-1 || !m_blocks[x][y+1][z].is_active()) {
                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 1.0f, 0.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{x*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{0.0f, 1.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, (z+1)*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 1.0f, 1.0f});

                    mesh_vertices.emplace_back(
                        std::array<float,3>{(x+1)*Block::BLOCK_SIZE, (y+1)*Block::BLOCK_SIZE, z*Block::BLOCK_SIZE});
                    mesh_vertices.emplace_back(std::array<float,3>{1.0f, 1.0f, 0.0f});

                    mesh_indices.insert(mesh_indices.end(), {idx, idx+1, idx+2, idx+2, idx+3, idx});
                    idx += 4;
                }
            }
        }
    }

    m_va = std::make_unique<VertexArray>();
    m_va->bind();
    m_vb = std::make_unique<VertexBuffer>(mesh_vertices.data(), 3*sizeof(float)*mesh_vertices.size());
    m_lay = std::make_unique<VertexBufferLayout>();
    m_lay->push<float>(3);//pos
    m_lay->push<float>(3);//color
    m_va->add_buffer(*m_vb, *m_lay);
    m_ib = std::make_unique<IndexBuffer>(mesh_indices.data(), mesh_indices.size());

    m_va->unbind();
    m_vb->unbind();
    m_ib->unbind();
}
void Chunk::update() {
}