#ifndef VOXEL_SIMULATION_H
#define VOXEL_SIMULATION_H

#include "../World/ChunkManager.h"
#include "../Physics/Physics.h"
#include "../Sensors/Heightmap.h"
#include <memory>

struct Observation {
    HeightmapReading heightmap;
    glm::vec3 position;
    glm::vec3 velocity;
    bool is_grounded = false;
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
    static constexpr int STEPS_PER_ACTION = 3;
    static constexpr int ACTIONS_PER_EP = 1200;
    int action_cnt;
public:
    static constexpr float DT = 1.0f/60.0f;
    static constexpr float ACTION_DT = STEPS_PER_ACTION * DT;
    Simulation(const TerrainSettings& settings);
    Simulation();
    ChunkManager& getChunk();
    int add_agent(glm::vec2 position);  //return agentId for future use
    void step(const std::vector<InputAction>& actions);
    void reset(unsigned int seed);
    const PBody& get_agent(int agentId) const;
    StepResult observe(int agentId) const;
};

#endif //VOXEL_SIMULATION_H
