#include "Simulation.h"
#include <cmath>
#include <optional>

Simulation::Simulation(const TerrainSettings& settings)
    : m_chunkman(std::make_unique<ChunkManager>(settings)), settings(settings), action_cnt(0)
{}

Simulation::Simulation() : Simulation(TerrainSettings{16.0f, 0.01f, 42}) {}

ChunkManager& Simulation::getChunk() {
    return *m_chunkman;
}

int Simulation::add_agent(glm::vec2 position) {
    const int world_x = static_cast<int>(std::floor(position.x));
    const int world_z = static_cast<int>(std::floor(position.y));
    std::optional<int> surfaceY = m_chunkman->surface_height(world_x, world_z);
    int id = -1;
    if(surfaceY){
        glm::vec3 pos{position.x, static_cast<float>(*surfaceY), position.y};
        id = m_physics.add_agent(pos);
        m_starting_positions[id] = position;
    }
    return id;
}

void Simulation::step(const std::vector<InputAction>& actions) {
    static_assert(STEPS_PER_ACTION > 0, "STEPS_PER_ACTION must be positive");
    for(int i = 0; i < STEPS_PER_ACTION; i++) {
        for(const InputAction& action : actions) {
            m_physics.move_agent(action, DT);
        }
        m_physics.step(*m_chunkman, DT);
    }
    ++action_cnt;
}

void Simulation::reset(unsigned int seed) {
    const TerrainSettings new_settings{settings.amplitude, settings.frequency, seed};
    auto new_chunkman = std::make_unique<ChunkManager>(new_settings);
    m_chunkman = std::move(new_chunkman);
    settings = new_settings;
    action_cnt = 0;
    m_physics.reset();
    std::unordered_map<int, glm::vec2> temp;
    temp.swap(m_starting_positions);
    for(const auto& [_, pos] : temp) {
        add_agent(pos);
    }
}

const PBody& Simulation::get_agent(int agentId) const {
    return m_physics.get_agent(agentId);
}

StepResult Simulation::observe(int agentId) const {
    const PBody& agent = m_physics.get_agent(agentId);
    Heightmap sensor;
    return {
        {
            sensor.sense(*m_chunkman, agent.position),
            agent.position,
            agent.velocity,
            agent.is_grounded
        },
        0.0f,      // TODO: reward system
        false,  // TODO
        action_cnt >= ACTIONS_PER_EP
    };
}
