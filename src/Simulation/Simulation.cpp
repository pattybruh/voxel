#include "Simulation.h"

Simulation::Simulation(const TerrainSettings& settings) : m_chunkman(settings), action_cnt(0) {}

Simulation::Simulation() : Simulation(TerrainSettings{16.0f, 0.01f, 42}) {}

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
    static_assert(STEPS_PER_ACTION > 0, "STEPS_PER_ACTION must be positive");
    for(int i = 0; i < STEPS_PER_ACTION; i++) {
        for(const InputAction& action : actions) {
            m_physics.move_agent(action, DT);
        }
        m_physics.step(m_chunkman, DT);
    }
    ++action_cnt;
}

void Simulation::reset() {
    action_cnt = 0;
    // TODO: reset agents to inital positions
}

const PBody& Simulation::get_agent(int agentId) const {
    return m_physics.get_agent(agentId);
}

StepResult Simulation::observe(int agentId) const {
    const PBody& agent = m_physics.get_agent(agentId);
    Heightmap sensor;
    return {
        {
            sensor.sense(m_chunkman, agent.position),
            agent.position,
            agent.velocity,
            agent.is_grounded
        },
        0,      // TODO: reward system
        false,  // TODO
        action_cnt >= ACTIONS_PER_EP
    };
}
