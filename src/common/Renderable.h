#pragma once

#include "raylib.h"

namespace swarmgame
{
namespace vis
{
enum class RenderableType
{
    UNKNOWN,
    UAV
};

struct Renderable
{
    Renderable(const RenderableType& type) : mRenderType(type)
    {}

    RenderableType mRenderType { RenderableType::UNKNOWN };
    virtual void render() const = 0;

    Vector3 toVisFrame(float x, float y, float z)
    {
        return {x, z, -y};
    }
};  
}
}