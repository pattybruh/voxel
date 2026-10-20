#ifndef VOXEL_SIMULATION_H
#define VOXEL_SIMULATION_H

#include "../World/ChunkManager.h"
#include "../Physics/Physics.h"
#include "../Sensors/Heightmap.h"
#include <cstdint>
#include <memory>
#include <optional>

struct Observation {
    HeightmapReading heightmap;
    glm::vec3 position;
    glm::vec3 velocity;
    bool is_grounded = false;
    glm::vec3 goal_position;
};

struct StepResult {
    Observation observation;
    float reward;
    bool terminated;
    bool truncated;
};

class Simulation {
private:
    std::unique_ptr<ChunkManager> m_chunkman;
    Physics m_physics;
    TerrainSettings settings;
    std::unordered_map<int, glm::vec2> m_starting_positions;
    glm::vec2 m_flag_xz;
    glm::vec3 m_flag_pos;
    static constexpr int STEPS_PER_ACTION = 3;
    static constexpr int ACTIONS_PER_EP = 1200;
    int action_cnt;
    bool m_episode_terminated;

    std::optional<glm::vec3> place_at(const glm::vec2& pos) const;
    float distance_to_flag(int agentId) const;
    bool touches_flag(int agentId) const;
public:
    static constexpr float DT = 1.0f/60.0f;
    static constexpr float ACTION_DT = STEPS_PER_ACTION * DT;
    Simulation(const TerrainSettings& settings, const glm::vec2& flag_pos);
    Simulation();
    ChunkManager& getChunk();
    int add_agent(glm::vec2 position);
    bool has_agent(uint32_t agentId) const;
    int agent_cnt() const;
    std::optional<int> sole_agent_id() const;
    std::optional<StepResult> step(const InputAction& action);
    bool reset(unsigned int seed);
    const PBody& get_agent(int agentId) const;
    glm::vec3 flag_position() const;
    StepResult observe(int agentId) const;
};

#endif //VOXEL_SIMULATION_H
