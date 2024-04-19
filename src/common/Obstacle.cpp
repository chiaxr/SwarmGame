#include "common/Obstacle.h"

namespace swarmgame
{
namespace common
{
Obstacle::Obstacle()
    : vis::Renderable(vis::RenderableType::OBSTACLE)
{}

Obstacle::Obstacle(
        const std::vector<Vector2>& polygon,
        const float minHeight,
        const float maxHeight)
    :
    vis::Renderable(vis::RenderableType::OBSTACLE),
    mPolygon(polygon), mMinHeight(minHeight), mMaxHeight(maxHeight)
{
    setTriangles(mPolygon, mMinHeight, mMaxHeight);
}

Obstacle::Obstacle(const nlohmann::json& json)
    : vis::Renderable(vis::RenderableType::OBSTACLE)
{
    for (const nlohmann::json& v : json.at("polygon"))
    {
        mPolygon.emplace_back(
            static_cast<float>(v.at("x")),
            static_cast<float>(v.at("y"))
        );
    }

    mMinHeight = static_cast<float>(json.at("minHeight"));
    mMaxHeight = static_cast<float>(json.at("maxHeight"));

    setTriangles(mPolygon, mMinHeight, mMaxHeight);
}

void Obstacle::setTriangles(
    std::vector<Vector2>& polygon,
    const float minHeight,
    const float maxHeight)
{
    mTriangles.clear();
    sortVerticesCounterclockwise(polygon);

    // Side faces of obstacle
    for (size_t i = 0; i < polygon.size(); i++)
    {
        // Side face is a quadrilateral that can be split into 2 triangles
        // d   c
        // X---X
        // |\  |
        // | \ |
        // |  \|
        // X---X
        // a   b

        const size_t j = (i + 1) % polygon.size(); // handle wraparound
        const Vector2 v1 = polygon.at(i);
        const Vector2 v2 = polygon.at(j);

        const Vector3 a { v1.x, v1.y, minHeight };
        const Vector3 b { v2.x, v2.y, minHeight };
        const Vector3 c { v2.x, v2.y, maxHeight };
        const Vector3 d { v1.x, v1.y, maxHeight };

        mTriangles.emplace_back(a, b, d);
        mTriangles.emplace_back(b, c, d);
    }
    
    // Top and bottom of obstacle
    std::vector<Triangle<Vector2>> triangles2d = triangulate(polygon);
    for (const auto& triangle : triangles2d)
    {
        // Top
        // Normals facing up
        mTriangles.emplace_back(
            Vector3({triangle.v0.x, triangle.v0.y, maxHeight}),
            Vector3({triangle.v1.x, triangle.v1.y, maxHeight}),
            Vector3({triangle.v2.x, triangle.v2.y, maxHeight})
        );

        // Bottom
        // Normals facing down
        mTriangles.emplace_back(
            Vector3({triangle.v2.x, triangle.v2.y, minHeight}),
            Vector3({triangle.v1.x, triangle.v1.y, minHeight}),
            Vector3({triangle.v1.x, triangle.v0.y, minHeight})
        );
    }
}

void Obstacle::render() const
{
    for (const auto& triangle : mTriangles)
    {
        DrawTriangle3D(toVisFrame(triangle.v0), toVisFrame(triangle.v1), toVisFrame(triangle.v2), LIGHTGRAY);
    }
}
}
}