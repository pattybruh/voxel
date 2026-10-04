#include "Simulation.h"

Simulation::Simulation(const TerrainSettings& settings) : m_chunkman(settings) {}

Simulation::Simulation() : m_chunkman(TerrainSettings{16.0f, 0.01f, 42})
{}

ChunkManager& Simulation::getChunk() {
    return m_chunkman;
}

int Simulation::add_agent(glm::vec3 position) {
    int id = m_physics.add_agent(position);
    m_starting_positions[id] = position;
    return id;
}

// TODO: make sure 'actions' is sanitized on creation from gRPC input
void Simulation::step(const std::vector<InputAction>& actions) {
    for(const InputAction& action : actions) {
        m_physics.move_agent(action, DT);
    }
    m_physics.step(m_chunkman, DT);
}

void Simulation::reset() {
}

glm::vec3 Simulation::get_agent_pos(int agentId) const {
    return m_physics.get_pos(agentId);
}