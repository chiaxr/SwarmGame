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
        mUavRadius = params.at("uavRadius");
        mUavMaxSpeed = params.at("uavMaxSpeed");

        mAngleStep = params.at("lidar2d").at("angleStep");
        mMaxRange = params.at("lidar2d").at("maxRange");
    }

    // UAV
    float mUavRadius { 0.5f };
    float mUavMaxSpeed { 1.0f };

    // 2D LIDAR
    float mAngleStep { 360.0f };
    float mMaxRange { 0.0f };
};  
}
}