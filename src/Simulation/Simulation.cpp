#include "Simulation.h"
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

Simulation::Simulation(const TerrainSettings& settings, const glm::vec2& flag_pos)
    : m_chunkman(std::make_unique<ChunkManager>(settings)),
    settings(settings),
    m_flag_xz(flag_pos),
    m_flag_pos({0, 0, 0}),
    action_cnt(0),
    m_episode_terminated(false)
{
    auto flagCoord = place_at(m_flag_xz);
    if (!flagCoord) {
        m_flag_xz = glm::vec2{0.0f, 0.0f};
        flagCoord = place_at(m_flag_xz);
    }
    m_flag_pos = *flagCoord;
}

Simulation::Simulation()
    : Simulation(TerrainSettings{16.0f, 0.01f, 42},
        glm::vec2{ChunkManager::WORLD_RADIUS*10, ChunkManager::WORLD_RADIUS*4})
{}


std::optional<glm::vec3> Simulation::place_at(const glm::vec2& pos) const {
    const double world_x_float = std::floor(static_cast<double>(pos.x));
    const double world_z_float = std::floor(static_cast<double>(pos.y));
    if (!std::isfinite(world_x_float) || !std::isfinite(world_z_float) ||
        world_x_float < std::numeric_limits<int>::min() ||
        world_x_float > std::numeric_limits<int>::max() ||
        world_z_float < std::numeric_limits<int>::min() ||
        world_z_float > std::numeric_limits<int>::max()) {
        return std::nullopt;
    }
    std::optional<int> surfaceY = m_chunkman->surface_height(static_cast<int>(world_x_float), static_cast<int>(world_z_float));
    if(surfaceY){
        return glm::vec3{pos.x, static_cast<float>(*surfaceY), pos.y};
    }
    return std::nullopt;
}

float Simulation::distance_to_flag(int agentId) const {
    const glm::vec3& position = m_physics.get_agent(agentId).position;
    return glm::distance(glm::vec2{position.x, position.z},
                         glm::vec2{m_flag_pos.x, m_flag_pos.z});
}

bool Simulation::touches_flag(int agentId) const {
    return glm::distance(m_physics.get_agent(agentId).position, m_flag_pos) < 1.0f;
}


ChunkManager& Simulation::getChunk() {
    return *m_chunkman;
}

int Simulation::add_agent(glm::vec2 position) {
    auto surfaceCoord = place_at(position);
    int id = -1;
    if(surfaceCoord){
        id = m_physics.add_agent(*surfaceCoord);
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

std::optional<StepResult> Simulation::step(const InputAction& action) {
    static_assert(STEPS_PER_ACTION > 0, "STEPS_PER_ACTION must be positive");
    if(m_episode_terminated) {
        return std::nullopt;
    }
    const float d_before = distance_to_flag(action.agentId);
    for(int i = 0; i < STEPS_PER_ACTION; i++) {
        m_physics.move_agent(action, DT);
        m_physics.step(*m_chunkman, DT);
    }
    ++action_cnt;
    const float d_after = distance_to_flag(action.agentId);
    StepResult result = observe(action.agentId);
    result.terminated = touches_flag(action.agentId);
    result.reward = 0.1f * (d_before - d_after) - 0.01f;
    if(result.terminated) {
        result.reward += 10.0f;
    }
    m_episode_terminated = result.terminated || result.truncated;
    return result;
}

bool Simulation::reset(unsigned int seed) {
    const TerrainSettings new_settings{settings.amplitude, settings.frequency, seed};
    auto new_chunkman = std::make_unique<ChunkManager>(new_settings);
    std::vector<std::pair<int, glm::vec3>> spawn_positions;
    spawn_positions.reserve(m_starting_positions.size());
    m_chunkman.swap(new_chunkman);
    for (const auto& [id, position] : m_starting_positions) {
        auto new_pos = place_at(position);
        if(!new_pos){
            m_chunkman.swap(new_chunkman);
            return false;
        }
        spawn_positions.emplace_back(id, *new_pos);
    }

    const auto flag_pos = place_at(m_flag_xz);
    if (!flag_pos) {
        m_chunkman.swap(new_chunkman);
        return false;
    }

    settings = new_settings;
    m_flag_pos = *flag_pos;
    action_cnt = 0;
    m_episode_terminated = false;
    for (const auto& [id, position] : spawn_positions) {
        m_physics.reset_agent(id, position);
    }
    return true;
}

const PBody& Simulation::get_agent(int agentId) const {
    return m_physics.get_agent(agentId);
}

glm::vec3 Simulation::flag_position() const {
    return m_flag_pos;
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
        0.0f,      // Observing alone does not earn a reward.
        false,  // TODO
        action_cnt >= ACTIONS_PER_EP
    };
}
