#pragma once

#include "common/Obstacle.h"
#include "common/Uav.h"

#include "raymath.h"

#include <vector>

namespace swarmgame
{
namespace sim
{
class Lidar2d
{
public:
    static std::vector<Vector3> simulate(
        const Vector3& pos,
        const std::vector<common::Triangle<Vector3>>& triangles,
        const float angleStep,
        const float maxRange);
};
}
}