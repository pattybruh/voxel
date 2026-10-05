#include "Heightmap.h"


HeightmapReading Heightmap::sense(const ChunkManager &chunkman, const glm::vec3 &position) const {
    HeightmapReading reading;
    static_assert(SIZE > 0 && (SIZE % 2 == 1), "Heightmap size must be odd");
    reading.heights = std::vector<std::vector<float>>(SIZE, std::vector<float>(SIZE, 0));
    reading.valid = std::vector<std::vector<bool>>(SIZE, std::vector<bool>(SIZE, false));
    int half_size = SIZE / 2;
    for(int dx = -half_size; dx <= half_size; dx++) {
        for(int dz = -half_size; dz <= half_size; dz++) {
            int world_x = static_cast<int>(std::floor(position.x)) + dx;
            int world_z = static_cast<int>(std::floor(position.z)) + dz;
            auto height_opt = chunkman.surface_height(world_x, world_z);
            if(height_opt) {
                reading.heights[dx + half_size][dz + half_size] = static_cast<float>(*height_opt) - position.y;
                reading.valid[dx + half_size][dz + half_size] = true;
            }
            else {
                reading.valid[dx + half_size][dz + half_size] = false;
            }
        }
    }
    return reading;
}