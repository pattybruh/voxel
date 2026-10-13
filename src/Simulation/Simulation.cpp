#include "Simulation.h"
#include <cmath>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

Simulation::Simulation(const TerrainSettings& settings)
    : m_chunkman(std::make_unique<ChunkManager>(settings)), settings(settings), action_cnt(0)
{}

Simulation::Simulation() : Simulation(TerrainSettings{16.0f, 0.01f, 42}) {}

ChunkManager& Simulation::getChunk() {
    return *m_chunkman;
}

int Simulation::add_agent(glm::vec2 position) {
    const double world_x_float = std::floor(static_cast<double>(position.x));
    const double world_z_float = std::floor(static_cast<double>(position.y));
    if (!std::isfinite(world_x_float) || !std::isfinite(world_z_float) ||
        world_x_float < std::numeric_limits<int>::min() ||
        world_x_float > std::numeric_limits<int>::max() ||
        world_z_float < std::numeric_limits<int>::min() ||
        world_z_float > std::numeric_limits<int>::max()) {
        return -1;
    }
    const int world_x = static_cast<int>(world_x_float);
    const int world_z = static_cast<int>(world_z_float);
    std::optional<int> surfaceY = m_chunkman->surface_height(world_x, world_z);
    int id = -1;
    if(surfaceY){
        glm::vec3 pos{position.x, static_cast<float>(*surfaceY), position.y};
        id = m_physics.add_agent(pos);
        m_starting_positions[id] = position;
    }
    return id;
}

bool Simulation::has_agent(uint32_t agentId) const {
    if (agentId > static_cast<uint32_t>(std::numeric_limits<int>::max())) {
        return false;
    }
    return m_starting_positions.find(static_cast<int>(agentId)) != m_starting_positions.end();
}

int Simulation::agent_cnt() const {
    return m_starting_positions.size();
}

std::optional<int> Simulation::sole_agent_id() const {
    if (m_starting_positions.size() != 1) {
        return std::nullopt;
    }
    return m_starting_positions.begin()->first;
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

bool Simulation::reset(unsigned int seed) {
    const TerrainSettings new_settings{settings.amplitude, settings.frequency, seed};
    auto new_chunkman = std::make_unique<ChunkManager>(new_settings);
    std::vector<std::pair<int, glm::vec3>> spawn_positions;
    spawn_positions.reserve(m_starting_positions.size());
    for (const auto& [id, position] : m_starting_positions) {
        const int world_x = static_cast<int>(std::floor(position.x));
        const int world_z = static_cast<int>(std::floor(position.y));
        const auto surface_y = new_chunkman->surface_height(world_x, world_z);
        if (!surface_y) {
            return false;
        }
        spawn_positions.emplace_back(id, glm::vec3{position.x, static_cast<float>(*surface_y), position.y});
    }

    m_chunkman = std::move(new_chunkman);
    settings = new_settings;
    action_cnt = 0;
    for (const auto& [id, position] : spawn_positions) {
        m_physics.reset_agent(id, position);
    }
    return true;
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
