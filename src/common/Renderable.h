#pragma once

#include "raylib.h"

namespace swarmgame
{
namespace vis
{
enum class RenderableType
{
    UAV,
    OBSTACLE,
    UNKNOWN
};

struct Renderable
{
    Renderable(const RenderableType& type) : mRenderType(type)
    {}

    RenderableType mRenderType { RenderableType::UNKNOWN };
    virtual void render() = 0;

    Vector3 toVisFrame(float x, float y, float z)
    {
        return {x, z, -y};
    }
};  
}
}