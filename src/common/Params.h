#pragma once

#include "nlohmann/json.hpp"

namespace swarmgame
{
namespace common
{
struct Params
{
    Params() = default;
    Params(const nlohmann::json& params)
    {
        mMessageTimeoutMs = params.at("messageTimeoutMs");
    }

    int mMessageTimeoutMs { -1 };
};  
}
}