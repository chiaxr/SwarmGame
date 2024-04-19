#pragma once

#include "common/JsonWrapper.h"
#include "common/Obstacle.h"
#include "common/Uav.h"

namespace swarmgame
{
namespace common
{
struct Scenario
{
    Scenario() = default;
    Scenario(const nlohmann::json& scenario)
    {
        if (scenario.contains("uavs"))
        {
            for (const nlohmann::json& uav : scenario.at("uavs"))
            {
                Uav temp;
                temp.fromJson(uav);
                mInitialUavs.emplace_back(std::move(temp));
            }
        }

        if (scenario.contains("obstacles"))
        {
            for (const nlohmann::json& obstacle : scenario.at("obstacles"))
            {
                mObstacles.emplace_back(obstacle);
            }
        }
    }

    std::vector<Uav> mInitialUavs;
    std::vector<Obstacle> mObstacles;
};  
}
}