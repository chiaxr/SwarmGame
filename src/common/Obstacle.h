#pragma once

#include "common/Geometry.h"
#include "common/Renderable.h"

#include "common/JsonWrapper.h"

namespace swarmgame
{
namespace common
{
// Extruded polygon obstacle
struct Obstacle : public vis::Renderable
{
    Obstacle();

    Obstacle(
        const std::vector<Vector2>& polygon,
        const float minHeight,
        const float maxHeight);

    Obstacle(const nlohmann::json& json);

    void render() const override;
    void setTriangles(
        std::vector<Vector2>& polygon,
        const float minHeight,
        const float maxHeight);

    std::vector<Vector2> mPolygon;
    float mMinHeight { 0.0f };
    float mMaxHeight { 0.0f };

    // For rendering and collision checking
    std::vector<Triangle<Vector3>> mTriangles;
};
}
}