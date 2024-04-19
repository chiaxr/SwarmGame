#pragma once

#include "common/JsonWrapper.h"

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