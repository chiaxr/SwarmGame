#pragma once

#include "common/Uav.h"

#include "common/JsonWrapper.h"

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
    }

    std::vector<Uav> mInitialUavs;
};  
}
}