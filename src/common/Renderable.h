#pragma once

#include "raylib.h"

namespace swarmgame
{
namespace vis
{
enum class RenderableType
{
    UNKNOWN,
    UAV,
    OBSTACLE
};

struct Renderable
{
    Renderable(const RenderableType& type) : mRenderType(type)
    {}

    RenderableType mRenderType { RenderableType::UNKNOWN };
    virtual void render() const = 0;

    Vector3 toVisFrame(const float x, const float y, const float z) const
    {
        return {x, z, -y};
    }

    Vector3 toVisFrame(const Vector3& v) const
    {
        return {v.x, v.z, -v.y};
    }
};
}
}