#ifndef VOXEL_SIMULATION_H
#define VOXEL_SIMULATION_H

#include "../World/ChunkManager.h"
#include "../Physics/Physics.h"

class Simulation {
private:
    ChunkManager m_chunkman;
    Physics m_physics;
    std::unordered_map<int, glm::vec3> m_starting_positions;
public:
    static constexpr float DT = 1.0f/60.0f;
    Simulation(const TerrainSettings& settings);
    Simulation();
    ChunkManager& getChunk();
    int add_agent(glm::vec3 position);  //return agentId for future use
    void step(const std::vector<InputAction>& actions);
    void reset();
    glm::vec3 get_agent_pos(int agentId) const;
};

#endif //VOXEL_SIMULATION_H
