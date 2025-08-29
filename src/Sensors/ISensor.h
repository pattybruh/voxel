//
// Created by patrick on 8/25/25.
//

#ifndef VOXEL_ISENSOR_H
#define VOXEL_ISENSOR_H

#include <glm/glm.hpp>
#include "../World/IWQueries.h"

class ISensor {
public:
    virtual ~ISensor() = default;
    virtual unsigned int get_id() const = 0;
    virtual double get_rate() const = 0;
    virtual void set_external(const glm::dmat4& w_sensor) = 0;
    virtual bool tick(double period, double time, const IWQueries& iwq) = 0;
};

#endif //VOXEL_ISENSOR_H