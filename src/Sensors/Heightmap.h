#ifndef SENSOR_HEIGHTMAP_H 
#define SENSOR_HEIGHTMAP_H 
#include <vector>
#include "../World/ChunkManager.h"

struct HeightmapReading{
    std::vector<std::vector<float>> heights;
    std::vector<std::vector<bool>> valids;
};

class Heightmap {
private:
public:
    static constexpr int SIZE = 9;
    Heightmap() = default;
    HeightmapReading sense(const ChunkManager& chunkman, const glm::vec3& position) const;
};

#endif //SENSOR_HEIGHTMAP_H