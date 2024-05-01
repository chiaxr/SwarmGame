#pragma once

#include "common/Params.h"

#include "common/JsonWrapper.h"

namespace swarmgame
{
namespace sim
{
struct SimulationParams : public common::Params
{
    SimulationParams() = default;
    SimulationParams(const nlohmann::json& params)
        : common::Params(params)
    {
        mAngleStep = params.at("lidar2d").at("angleStep");
        mMaxRange = params.at("lidar2d").at("maxRange");
    }

    // 2D LIDAR
    float mAngleStep { 360.0f };
    float mMaxRange { 0.0f };
};  
}
}