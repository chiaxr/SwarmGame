#pragma once

#include "common/JsonWrapper.h"

#include "raylib.h"

#include <vector>

namespace swarmgame
{
namespace sim
{
struct SensorData
{
    int32_t id { 0 };
    std::vector<Vector3> pointcloud;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        SensorData,
        id,
        pointcloud);
};
}
}