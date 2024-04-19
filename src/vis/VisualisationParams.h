#pragma once

#include "common/Params.h"

#include "common/JsonWrapper.h"

namespace swarmgame
{
namespace vis
{
struct VisualisationParams : public common::Params
{
    VisualisationParams() = default;
    VisualisationParams(const nlohmann::json& params)
        : common::Params(params)
    {
        mScreenWidth = params.at("screenWidth");
        mScreenHeight = params.at("screenHeight");
    }

    // Screen size
    int mScreenWidth { 0 };
    int mScreenHeight { 0 };
};  
}
}