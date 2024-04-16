#pragma once

#include "common/Uav.h"

#include "nlohmann/json.hpp"

namespace swarmgame
{
namespace common
{
struct Scenario
{
    Scenario() = default;
    Scenario(const nlohmann::json& scenario)
    {
        for (const nlohmann::json& uav : scenario.at("uavs"))
        {
            mInitialUavs.emplace_back(uav);
        }
    }

    // Scenario
    std::vector<Uav> mInitialUavs;
};  
}
}