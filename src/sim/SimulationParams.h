#pragma once

#include "common/Params.h"

#include "nlohmann/json.hpp"

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
        
    }

};  
}
}