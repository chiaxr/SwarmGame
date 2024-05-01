#pragma once

#include "Common/JsonWrapper.h"
#include "common/Renderable.h"

namespace swarmgame
{
namespace vis
{
struct Line : public Renderable
{
    Line() : Renderable(RenderableType::LINE)
    {}

    Line(const Vector3& start, const Vector3& end, const Color& color)
        : Renderable(RenderableType::LINE), mStart(start), mEnd(end), mColor(color)
    {}

    Vector3 mStart { 0.0f, 0.0f, 0.0f };
    Vector3 mEnd { 0.0f, 0.0f, 0.0f };
    Color mColor { RED };

    void render() const
    {
        DrawLine3D(toVisFrame(mStart), toVisFrame(mEnd), mColor);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        Line,
        mStart,
        mEnd,
        mColor);
};
}
}