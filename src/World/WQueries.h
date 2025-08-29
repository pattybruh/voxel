
#ifndef VOXEL_WQUERIESADAPTER_H
#define VOXEL_WQUERIESADAPTER_H
#include "IWQueries.h"

class ChunkManager;

class WQueries final : public IWQueries{
private:
    ChunkManager* m_chunkman;
    static constexpr double M_VOX = 1.0f; //meters/voxel

    inline double first_t(double o, int iw, int s, double dir) const;
public:
    explicit WQueries(ChunkManager* chunkman) : m_chunkman(chunkman){};

    RayHit raycast(const glm::dvec3 &origin, const glm::dvec3 &dir_unit_n, double max_range) const override;
};


#endif //VOXEL_WQUERIESADAPTER_H